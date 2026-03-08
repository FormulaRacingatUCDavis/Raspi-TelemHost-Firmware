<script lang="ts">
	import '@picocss/pico';
	import LogButton from '$lib/components/LogButton.svelte';
	let { children } = $props();
	import { mqttDataStr } from '$lib/mqtt.svelte';

	type Log = { file_name: string };
	let files = $state<Log[]>([]);
	let selectedFiles = $state([]);

	async function updateDropdown() {
		const res = await fetch('/api/can/raw-logs/list');
		const data: Log[] = await res.json();
		files = data;
	}

	async function download() {
        const response = await fetch('/api/can/raw-logs/zip', {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ filenames: selectedFiles })
        });

        const blob = await response.blob();
        const url = URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = "canlogs.zip";
        document.body.appendChild(a);
        a.click();
        a.remove();
        URL.revokeObjectURL(url);

		console.log(selectedFiles);
	}
</script>

<header class="container">
	<nav>
		<ul>
			<li>
				<img src="/logo.svg" alt="FRUCD Logo" style="height:70px; width:auto; padding-top:10px" />
			</li>
		</ul>
		<ul>
			<li><a href="/" class="secondary nav-link">Home</a></li>
			<li><a href="/live/graphs" class="secondary nav-link">Live</a></li>
			<li><a href="/logs/graphs" class="secondary nav-link">Logs</a></li>
			<li>
				<details class="dropdown" ontoggle={updateDropdown}>
					<summary> Export </summary>
					<ul id="logOptions" dir="rtl">
						{#if files.length > 0}
							{#each files as file}
								<li>
									<label id="logOptions" dir="ltr">
										<input
											type="checkbox"
											name="phase"
											value={file.file_name}
											bind:group={selectedFiles}
										/>
										{file.file_name}
									</label>
								</li>
							{/each}
							<!-- svelte-ignore a11y_invalid_attribute -->
							<li><a href="#" onclick={download}>Download Raw</a></li>
						{:else}
							<li>No logs available</li>
						{/if}
					</ul>
				</details>
			</li>
			<li>
				<LogButton
					control="can/log/control"
					status={$mqttDataStr.logStatus}
					request={$mqttDataStr.logReq}
				/>
			</li>
			<li>
				<LogButton
					control="camera/log/control"
					status={$mqttDataStr.recStatus}
					request={$mqttDataStr.recReq}
				/>
			</li>
		</ul>
	</nav>
	<hr />
</header>

<main id="main-content" class="container">
	{@render children()}
</main>
