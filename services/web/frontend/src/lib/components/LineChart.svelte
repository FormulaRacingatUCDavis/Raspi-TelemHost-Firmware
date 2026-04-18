<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';
	import 'chartjs-adapter-moment';

	let { value = 0, timestamp = Date.now(), label = 'Signal', maxPoints = 1000 } = $props();

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
			// 'none' mode skips all animation and extra logic
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
						// Optimization: Use a single color/width for the whole line
						borderColor: 'rgb(135, 139, 219)',
						borderWidth: 1,
						pointRadius: 0,
						pointHitRadius: 0,
						tension: 0,
						fill: false,
						spanGaps: false
					}
				]
			},
			options: {
				responsive: true,
				maintainAspectRatio: false,
				animation: false,
				// Optimization: Tell Chart.js data is already sorted and pre-parsed
				parsing: false,
				normalized: true,
				// Optimization: Disable all event listeners (hover, tooltip, etc)
				events: [],
				elements: {
					line: {
						capBezierPoints: false // Prevents extra calc for line caps
					}
				},
				scales: {
					x: {
						type: 'time',
						time: { unit: 'second' },
						ticks: {
							source: 'auto',
							maxRotation: 0,
							autoSkip: true,
							sampleSize: 1 // Only sample 1 tick for size calc
						},
						grid: { display: false } // Drawing less is faster
					},
					y: {
						beginAtZero: false,
						ticks: { precision: 2 },
						grid: { color: 'rgba(0,0,0,0.05)' }
					}
				},
				plugins: {
					legend: {
						display: true,
						position: 'top',
						align: 'center'
					},
					tooltip: { enabled: false }, // Major CPU saver
					decimation: {
						enabled: true,
						algorithm: 'min-max'
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
