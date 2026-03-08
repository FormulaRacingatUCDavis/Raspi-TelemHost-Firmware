<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';
	import 'chartjs-adapter-moment';

	type Log = { file_name: string; creation_date: number; file_size: number };
	let files = $state<Log[]>([]);
	let selectedLog = $state('');
	let selectedSignal = $state('');
	let lineChart: Chart;
	let lineCanvas: HTMLCanvasElement;

	let timeElapsed = $state(0);

	const lineOptions = [
		{ key: 'dashboard_torque', label: 'Torque [Nm]' },
		{ key: 'inv_motor_speed', label: 'Motor Speed [RPM]' },
		{ key: 'pei_soc', label: 'State of Charge [%]' }
	];

	let lineData = {
		datasets: [
			{
				label: 'No data selected',
				data: [],
				spanGaps: true,
				backgroundColor: 'rgb(135, 139, 219)',
				borderColor: 'rgb(135, 139, 219)',
				fill: false
			}
		]
	};

    function formatTime(ts: number) {
    return new Date(ts * 1000).toLocaleString();
    }

    function formatSize(bytes: number) {
    if (bytes < 1024) return `${bytes} B`;
    if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
    return `${(bytes / 1024 / 1024).toFixed(2)} MB`;
    }

	async function updateDropdown() {
		const res = await fetch('/api/can/logs/list');
		const data: Log[] = await res.json();
		files = data;
	}

    onMount(() => {
        (async () => {
            await updateDropdown();

            lineChart = new Chart(lineCanvas, { 
                type: 'line', 
                data: lineData, 
                options: { 
                    spanGaps: false,
                    parsing: false,
                    normalized: true,
                    scales: {
                        x: {
                            ticks: { sampleSize: 10, maxTicksLimit: 10 },
                            type: 'time',
                            time: { unit: 'second' },
                            title: { display: true, text: 'Time Elapsed [s]' }
                        },
                        y: { title: { display: true, text: 'No Data Selected' } }
                    }
                }
            });
        })();

        return () => {
            if (lineChart) lineChart.destroy();
        };
    });
</script>

<article>
	<select bind:value={selectedLog} onfocus={updateDropdown}>
		<option selected disabled value="">Select</option>
		{#if files.length > 0}
			{#each files as file}
				<option value={file.file_name}>
					{file.file_name}
				</option>
			{/each}
		{/if}
	</select>
	<hr />
	<small>
		<div class="overflow-auto" style="max-height: 300px;">
            <table>
            <thead>
                <tr>
                <th>Filename</th>
                <th>Date Created</th>
                <th>Size</th>
                </tr>
            </thead>
            <tbody>
                {#each files as file}
                <tr>
                    <th scope="row">{file.file_name}</th>
                    <td>{formatTime(file.creation_date)}</td>
                    <td>{formatSize(file.file_size)}</td>
                </tr>
                {/each}
            </tbody>
            </table>
		</div>
	</small>
</article>
<article data-theme="light">
	<label>
		Time Elapsed: {timeElapsed} seconds
		<input type="range" bind:value={timeElapsed} min="0" max="100" />
	</label>
	<table class="striped">
		<thead>
			<tr>
				<th scope="col">Source</th>
				<th scope="col">State</th>
			</tr>
		</thead>
		<tbody>
			<tr>
				<th scope="row">Vehicle Control Unit</th>
				<td></td>
			</tr>
			<tr>
				<th scope="row">Power Electronics Interface</th>
				<td></td>
			</tr>
			<tr>
				<th scope="row">Motor Controller</th>
				<td></td>
			</tr>
		</tbody>
		<thead>
			<tr>
				<th scope="col">Metric</th>
				<th scope="col">Value</th>
			</tr>
		</thead>
		<tbody>
			<tr>
				<th scope="row">Speed [MPH]</th>
				<td></td>
			</tr>
			<tr>
				<th scope="row">State of Charge [%]</th>
				<td></td>
			</tr>
			<tr>
				<th scope="row">Motor Controller Temperature [C]</th>
				<td></td>
			</tr>
			<tr>
				<th scope="row">Ground Level Voltage [V]</th>
				<td></td>
			</tr>
		</tbody>
	</table>
</article>
<article data-theme="light">
	<select
		bind:value={selectedSignal}
		onchange={() => {
			if (!lineChart) return;

			lineChart.data.datasets[0].data = [];
			lineChart.update();
		}}
	>
		<option selected disabled value="">Select</option>
		{#each lineOptions as signal}
			<option value={signal.key}>{signal.label}</option>
		{/each}
	</select>
	<form>
		<!-- svelte-ignore a11y_no_redundant_roles -->
		<fieldset role="group">
			<input type="number" name="number" placeholder="Minimum [s]" aria-label="Number" />
			<input type="number" name="number" placeholder="Maximum [s]" aria-label="Number" />
			<input type="submit" value="Graph" />
		</fieldset>
	</form>
	<canvas bind:this={lineCanvas}></canvas>
</article>
