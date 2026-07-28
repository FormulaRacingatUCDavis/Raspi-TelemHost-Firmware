<script lang="ts">
	import LogDisplay from '$lib/components/LogDisplay.svelte';
	type Log = { file_name: string };
	let files = $state<Log[]>([]);
	let selectedFiles = $state([]);

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

	let selectedLog = $state('');
</script>

<LogDisplay source={"can"} log={selectedLog} logPath={"raw"} />