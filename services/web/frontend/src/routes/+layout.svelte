<script lang="ts">
	import '@picocss/pico';
	import LogButton from '$lib/components/LogButton.svelte';
	let { children } = $props();
	import { mqttDataStr } from '$lib/mqtt.svelte';

	type Log = { file_name: string };
	let files = $state<Log[]>([]);
	let selectedFiles = $state([]);

	async function updateDropdown() {
		const res = await fetch('/api/can/logs/list');
		const data: Log[] = await res.json();
		files = data;
	}

	async function download(type: 'raw' | 'parsed') {
		const response = await fetch(`/api/can/logs/zip/${type}`, {
			method: 'POST',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify({ filenames: selectedFiles })
		});

		const blob = await response.blob();
		const url = URL.createObjectURL(blob);
		const a = document.createElement('a');
		a.href = url;
		a.download = 'canlogs.zip';
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
			<li><a href="/live/readings" class="secondary nav-link">Live</a></li>
			<li><a href="/logs/readings" class="secondary nav-link">Logs</a></li>
			<li>
				<details class="dropdown" ontoggle={updateDropdown}>
					<summary> Export </summary>
					<ul dir="rtl">
						{#if files.length > 0}
							{#each files as file}
								<li>
									<label dir="ltr">
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
							<li>
								<input
									class="secondary"
									type="button"
									onclick={() => download('raw')}
									value="Raw"
								/>
							</li>
							<li>
								<input
									class="secondary"
									type="button"
									onclick={() => download('parsed')}
									value="Parsed"
								/>
							</li>
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
