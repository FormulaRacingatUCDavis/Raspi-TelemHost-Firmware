<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';
	import 'chartjs-adapter-moment';

	let {
		value = 0,
		timestamp = Date.now(),
		label = 'Signal',
		maxPoints = 1000
	} = $props();

	let lineChart: Chart;
	let canvas: HTMLCanvasElement;

	$effect(() => {
		const currentLabel = label; 
		if (lineChart) {
			lineChart.data.datasets[0].label = currentLabel;
			lineChart.data.datasets[0].data = [];
			lineChart.update('none');
		}
	});

	$effect(() => {
		const val = value;
		const time = timestamp;

		if (lineChart && val !== undefined) {
			const dataset = lineChart.data.datasets[0].data;
			if (dataset.length > maxPoints) {
				dataset.shift();
			}
			dataset.push({ x: time, y: val });
			lineChart.update('none');
		}
	});

	onMount(() => {
		lineChart = new Chart(canvas, {
			type: 'line',
			data: {
				datasets: [
					{
						label,
						data: [],
						spanGaps: true
					}
				]
			},
			options: {
				responsive: true,
				maintainAspectRatio: false,
				animation: false,
				parsing: false,
				scales: {
					x: { type: 'time', time: { unit: 'second' } },
					y: {
						beginAtZero: false,
						ticks: { precision: 2 }
					}
				},
				plugins: {
					legend: {
						display: true,
						position: 'top',
						align: 'center'
					}
				}
			}
		});

		return () => lineChart.destroy();
	});
</script>

<article data-theme="light">
	<div class="chart-wrapper">
		<canvas bind:this={canvas}></canvas>
	</div>
</article>

<style>
	article {
		padding: 1rem;
		margin-bottom: 0;
		display: flex;
		flex-direction: column;
	}
	.chart-wrapper {
		position: relative;
		flex-grow: 1;
		width: 100%;
		min-height: 350px;
	}
</style>