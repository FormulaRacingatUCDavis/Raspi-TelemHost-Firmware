<script lang="ts">
	import { onMount } from 'svelte';
	import mqtt from 'mqtt';

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
	let logging = $state(false);
	let mqttConnected = $state(false);

	let mqttClient: mqtt.MqttClient;

	function formatTime(ts: number) {
		return new Date(ts * 1000).toLocaleString();
	}

	function formatSize(bytes: number) {
		if (bytes < 1024) return `${bytes} B`;
		if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
		return `${(bytes / 1024 / 1024).toFixed(2)} MB`;
	}

	function startLogging() {
		if (!mqttClient || !mqttConnected) {
			console.error('[MQTT] Cannot start logger: MQTT not connected.');
			return;
		}

		const payload = JSON.stringify({ status: 'on' });
		mqttClient.publish('can/log/control', payload);
	}

	function stopLogging() {
		if (!mqttClient || !mqttConnected) {
			console.error('[MQTT] Cannot stop logger: MQTT not connected.');
			return;
		}

		const payload = JSON.stringify({ status: 'off' });
		mqttClient.publish('can/log/control', payload);
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
	}

	onMount(() => {
		mqttClient = mqtt.connect('ws://192.168.105.2:8080');
		mqttClient.on('connect', () => {
			console.log('[MQTT] Connected to broker.');
			mqttConnected = true;

			mqttClient.subscribe('can/log/status');
		});

		mqttClient.on('message', (topic, message) => {
			if (topic === 'can/log/status') {
				const data = JSON.parse(message.toString());
				logging = data.status === 'on';
			}
		});

		async function loadFiles() {
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
		}

		loadFiles();

		return () => {
			mqttClient.end();
		};
	});
</script>

<form
	onsubmit={(e) => {
		e.preventDefault();
	}}
>
	<fieldset class="grid">
		<input placeholder="Enter driver name (optional)" />

		<div class="grid">
			<input
				type="button"
				value="Record"
				disabled={logging || !mqttConnected}
				onclick={startLogging}
			/>

			<input
				type="button"
				class="secondary"
				value="Stop"
				disabled={!logging || !mqttConnected}
				onclick={stopLogging}
			/>
		</div>
	</fieldset>
</form>

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