<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';
	import { mqttDataNum, mqttDataStr } from '$lib/mqtt.svelte';
	import LineChart from '$lib/components/LineChart.svelte';

	const lineOptions = [
		{ key: 'dashboard_torque', label: 'Torque [Nm]' },
		{ key: 'inv_motor_speed', label: 'Motor Speed [RPM]' },
		{ key: 'pei_soc', label: 'State of Charge [%]' },
		{ key: 'acc_power', label: 'Acc Power [Watts]' },
		{ key: 'inlet_water_temp', label: 'Inlet Water Temp [C]' },
		{ key: 'outlet_water_temp', label: 'Outlet Water Temp [C]' }
	];

	let chartSlots = $state([{ key: 'dashboard_torque' }, { key: 'inv_motor_speed' }]);

	let mcChart: Chart;
	let coolingChart: Chart;
	let mcCanvas: HTMLCanvasElement;
	let coolingCanvas: HTMLCanvasElement;

	onMount(() => {
		mcChart = new Chart(mcCanvas, {
			type: 'bar',
			data: {
				labels: ['Module A', 'Module B', 'Module C'],
				datasets: [
					{
						label: 'MC Temps [C]',
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
			},
			options: { responsive: true, maintainAspectRatio: false }
		});

		coolingChart = new Chart(coolingCanvas, {
			type: 'bar',
			data: {
				labels: ['Inlet', 'Outlet'],
				datasets: [
					{
						label: 'Cooling Temps [C]',
						data: [0, 0],
						backgroundColor: ['rgba(54, 162, 235, 0.2)', 'rgba(255, 159, 64, 0.2)'],
						borderColor: ['rgb(54, 162, 235)', 'rgb(255, 159, 64)'],
						borderWidth: 1
					}
				]
			},
			options: { responsive: true, maintainAspectRatio: false }
		});

		const unsubscribeNum = mqttDataNum.subscribe((data) => {
			if (mcChart) {
				mcChart.data.datasets[0].data = [
					data.moduleTempA ?? 0,
					data.moduleTempB ?? 0,
					data.moduleTempC ?? 0
				];
				mcChart.update('none');
			}
			if (coolingChart) {
				coolingChart.data.datasets[0].data = [
					data.inlet_water_temp ?? 0,
					data.outlet_water_temp ?? 0
				];
				coolingChart.update('none');
			}
		});

		return () => {
			mcChart.destroy();
			coolingChart.destroy();
			unsubscribeNum();
		};
	});
</script>

<div class="grid">
	<article data-theme="light">
		<small>
			<table class="striped">
				<thead>
					<tr><th scope="col">Source</th><th scope="col">State</th></tr>
				</thead>
				<tbody>
					<tr><th scope="row">Vehicle Control Unit</th><td>{$mqttDataStr.vcuState}</td></tr>
					<tr><th scope="row">Power Electronics Interface</th><td>{$mqttDataStr.bmsState}</td></tr>
					<tr><th scope="row">Motor Controller</th><td>{$mqttDataStr.mcState}</td></tr>
				</tbody>
				<thead>
					<tr><th scope="col">Metric</th><th scope="col">Value</th></tr>
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
						<th scope="row">Ground Level Voltage [V]</th>
						<td></td>
					</tr>
				</tbody>
			</table>
		</small>
	</article>
	<div class="grid">
		<article data-theme="light"><canvas bind:this={mcCanvas}></canvas></article>
		<article data-theme="light"><canvas bind:this={coolingCanvas}></canvas></article>
	</div>
</div>

<div class="grid">
	{#each chartSlots as slot}
		<article data-theme="light" class="chart-card">
			<select bind:value={slot.key} data-theme="light">
				{#each lineOptions as opt}
					<option value={opt.key}>{opt.label}</option>
				{/each}
			</select>

			<LineChart
				value={$mqttDataNum[slot.key]}
				timestamp={$mqttDataNum.timestamp}
				label={lineOptions.find((o) => o.key === slot.key)?.label}
			/>
		</article>
	{/each}
</div>

<style>
	.chart-card {
		margin-bottom: var(--pico-block-spacing-vertical);
		overflow: visible;
	}
	select {
		margin-bottom: 1rem;
	}
</style>
