<script lang="ts">
	import { onMount } from 'svelte';

	type Log = { file_name: string; creation_date: number; file_size: number };
	let files = $state<Log[]>([]);
	let selectedFiles = $state([]);

	function formatTime(ts: number) {
		return new Date(ts * 1000).toLocaleString();
	}

	function formatSize(bytes: number) {
		if (bytes < 1024) return `${bytes} B`;
		if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
		return `${(bytes / 1024 / 1024).toFixed(2)} MB`;
	}

	async function updateDropdown() {
		const res = await fetch(`/api/can/logs/list/raw`);
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

	onMount(() => {
		updateDropdown();
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
					{#each files as file}
						<tr>
							<th scope="row">
								<label>
									<input type="checkbox" name="english" />
									{file.file_name}
								</label>
							</th>
							<td>{formatTime(file.creation_date)}</td>
							<td>{formatSize(file.file_size)}</td>
						</tr>
					{/each}
				</tbody>
			</table>
		</div>
	</small>
	<hr />
	<small>
		<fieldset>
			<label>
				<input type="checkbox" name="raw" />
				Raw (R)
			</label>
			<label>
				<input type="checkbox" name="parsed" />
				Parsed (P)
			</label>
		</fieldset>
	</small>
	<input type="button" value="Download" disabled/>
</article>