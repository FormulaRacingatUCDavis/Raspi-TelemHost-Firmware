<script lang="ts">
	type Log = { file_name: string };
	let files = $state<Log[]>([]);
	let selectedFiles = $state([]);

	let { children } = $props();

	async function updateDropdown() {
		const res = await fetch('/api/can/logs/list/raw');
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
		if (type === 'raw')
			a.download = 'canlogs-raw.zip';
		else
			a.download = 'canlogs-parsed.zip';
		document.body.appendChild(a);
		a.click();
		a.remove();
		URL.revokeObjectURL(url);

		console.log(selectedFiles);
	}
</script>

<nav>
	<ul>
		<li><strong>Logs</strong></li>
	</ul>
	<ul>
		<li><a href="/logs/readings" data-sveltekit-reload>Readings</a></li>
		<li><a href="/logs/camera" data-sveltekit-reload>Camera</a></li>
		<li>
			<details class="dropdown" ontoggle={updateDropdown}>
				<summary> Export CSV </summary>
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
							<input class="secondary" type="button" onclick={() => download('raw')} value="Raw" />
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
	</ul>
</nav>
{@render children?.()}
