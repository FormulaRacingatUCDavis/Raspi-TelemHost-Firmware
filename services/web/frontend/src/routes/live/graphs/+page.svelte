<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';
	import 'chartjs-adapter-moment';
	import { mqttDataNum, mqttDataStr } from '$lib/mqtt.svelte';

	let selectedSignal = $state('');
	let lineChart: Chart;
	let barChart: Chart;

	let lineCanvas: HTMLCanvasElement;
	let barCanvas: HTMLCanvasElement;

	const options = [
		{ key: 'dashboard_torque', label: 'Torque [Nm]' },
		{ key: 'inv_motor_speed', label: 'Motor Speed [RPM]' },
		{ key: 'pei_soc', label: 'State of Charge [%]' }
	];

	let lineData = {
		datasets: [
			{
				data: [],
				spanGaps: true,
				backgroundColor: 'rgb(135, 139, 219)',
				borderColor: 'rgb(135, 139, 219)',
				fill: false
			}
		]
	};
	let barData = {
		labels: ['Module A', 'Module B', 'Module C'],
		datasets: [
			{
				label: 'Inverter Module Temperatures',
				data: [0, 0, 0],
				backgroundColor: [
					'rgba(255, 99, 132, 0.2)',
					'rgba(255, 159, 64, 0.2)',
					'rgba(255, 205, 86, 0.2)'
				],
				borderColor: ['rgb(255, 99, 132)', 'rgb(255, 159, 64)', 'rgb(255, 205, 86)'],
				borderWidth: 1
			}
		]
	};

	$effect(() => {
		if (!lineChart) return;
		if (!selectedSignal) return;

		lineChart.data.datasets[0].data = [];
		lineChart.update('none');
	});

	onMount(() => {
		lineChart = new Chart(lineCanvas, {
			type: 'line',
			data: lineData,
			options: {
				spanGaps: false,
				parsing: false,
				normalized: true,
				scales: {
					x: {
						ticks: {
							sampleSize: 10,
							maxTicksLimit: 10
						},
						type: 'time',
						time: {
							unit: 'second'
						},
						title: {
							display: false
						}
					},
					y: {
						title: {
							display: false
						}
					}
				},
				plugins: {
					legend: {
						display: false
					}
				}
			}
		});
		barChart = new Chart(barCanvas, {
			type: 'bar',
			data: barData,
			options: {
				scales: {
					y: { beginAtZero: true, title: { display: true, text: 'Temperature [C]' } }
				},
				plugins: {
					legend: {
						display: false
					}
				}
			}
		});

		const unsubscribeNum = mqttDataNum.subscribe((data) => {
			if (barChart) {
				barChart.data.datasets[0].data = [
					data['moduleTempA'] ?? 0,
					data['moduleTempB'] ?? 0,
					data['moduleTempC'] ?? 0
				];
				barChart.update();
			}
			if (lineChart && selectedSignal in data) {
				const dataset = lineChart.data.datasets[0].data;
				if (dataset.length > 1000) {
					dataset.splice(0, dataset.length - 1000);
				}
				dataset.push({ x: data['timestamp'], y: data[selectedSignal] });
				lineChart.update('none');
			}
		});

		return () => {
			if (lineChart) lineChart.destroy();
			if (barChart) barChart.destroy();
			unsubscribeNum();
		};
	});
</script>

<h2>Graphs</h2>
<hr />
<article data-theme="light">
	<div class="grid">
		<div>
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
						<td>{$mqttDataStr.vcuState}</td>
					</tr>
					<tr>
						<th scope="row">Power Electronics Interface</th>
						<td>{$mqttDataStr.bmsState}</td>
					</tr>
					<tr>
						<th scope="row">Motor Controller</th>
						<td>{$mqttDataStr.mcState}</td>
					</tr>
				</tbody>
			</table>
		</div>
		<div>
			<canvas bind:this={barCanvas}></canvas>
		</div>
	</div>
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
		<option selected disabled value=""> Graph... </option>
		{#each options as signal}
			<option value={signal.key}>{signal.label}</option>
		{/each}
	</select>
	<canvas bind:this={lineCanvas}></canvas>
</article>
