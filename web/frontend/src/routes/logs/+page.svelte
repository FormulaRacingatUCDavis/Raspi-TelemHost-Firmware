<script lang="ts">
	import { onMount } from 'svelte';

	type Log = { file_name: string; creation_date: number; file_size: number };
	type FileInfo = {
		date: number;
		sizeRaw?: number;
		sizeParsed?: number;
	};

	let files = $state<Record<string, FileInfo>>({});
	let selectedFiles = $state<string[]>([]);

	let downloadRaw = $state(false);
	let downloadParsed = $state(false);

	function formatTime(ts: number) {
		return new Date(ts * 1000).toLocaleString();
	}

	function formatSize(bytes: number) {
		if (bytes < 1024) return `${bytes} B`;
		if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
		return `${(bytes / 1024 / 1024).toFixed(2)} MB`;
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
		a.download = type === 'raw' ? 'canlogs-raw.zip' : 'canlogs-parsed.zip';

		document.body.appendChild(a);
		a.click();
		a.remove();
		URL.revokeObjectURL(url);

		console.log(selectedFiles);
	}

	onMount(async () => {
		const rRes = await fetch('/api/can/logs/list/raw');
		const pRes = await fetch('/api/can/logs/list/parsed');

		const rawFiles: Log[] = await rRes.json();
		const parsedFiles: Log[] = await pRes.json();

		const map: Record<string, FileInfo> = {};

		for (const file of rawFiles) {
			map[file.file_name] = {
				date: file.creation_date,
				sizeRaw: file.file_size
			};
		}

		for (const file of parsedFiles) {
			if (!map[file.file_name]) {
				map[file.file_name] = {
					date: file.creation_date
				};
			}
			map[file.file_name].sizeParsed = file.file_size;
		}

		files = map;
	});
</script>

<article data-theme="light">
	<small>
		<div class="overflow-auto" style="max-height: 250px;">
			<table>
				<thead>
					<tr>
						<th>Filename</th>
						<th>Date Created</th>
						<th>Size (R)</th>
						<th>Size (P)</th>
					</tr>
				</thead>
				<tbody>
					{#each Object.entries(files) as [filename, file]}
						<tr>
							<th scope="row">
								<label>
									<input
										type="checkbox"
										value={filename}
										onchange={(e) => {
											const checked = (e.currentTarget as HTMLInputElement).checked;

											if (checked) {
												selectedFiles = [...selectedFiles, filename];
											} else {
												selectedFiles = selectedFiles.filter((f) => f !== filename);
											}
										}}
									/>
									{filename}
								</label>
							</th>
							<td>{formatTime(file.date)}</td>
							<td>
								{file.sizeRaw !== undefined ? formatSize(file.sizeRaw) : '-'}
							</td>
							<td>
								{file.sizeParsed !== undefined ? formatSize(file.sizeParsed) : '-'}
							</td>
						</tr>
					{/each}
				</tbody>
			</table>
		</div>
	</small>

	<small>
		<fieldset>
			<label>
				<input type="checkbox" bind:checked={downloadRaw} />
				Raw (R)
			</label>
			<label>
				<input type="checkbox" bind:checked={downloadParsed} />
				Parsed (P)
			</label>
		</fieldset>
	</small>

	<input
		type="button"
		value="Download"
		disabled={!downloadRaw && !downloadParsed}
		onclick={async () => {
			if (downloadRaw) await download('raw');
			if (downloadParsed) await download('parsed');
		}}
	/>
</article>