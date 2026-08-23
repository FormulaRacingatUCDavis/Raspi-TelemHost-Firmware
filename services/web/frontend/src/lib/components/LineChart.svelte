<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';
	import 'chartjs-adapter-moment';

	let { data } = $props();

	let canvas: HTMLCanvasElement | null = $state(null);
	let chart: Chart | null = null;

	onMount(() => {
		if (canvas) {
			chart = new Chart(canvas, {
				type: 'line',
				data: data,
				options: {
					responsive: true,
					maintainAspectRatio: false,
					animation: false,
					parsing: false,
					normalized: true,
					events: [],
					elements: {
						line: {
							capBezierPoints: false
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
								sampleSize: 1
							},
							grid: { display: false }
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
						tooltip: { enabled: false },
						decimation: {
							enabled: true,
							algorithm: 'min-max'
						}
					}
				}
			});
		}

		return () => {
			chart?.destroy();
		};
	});

	$effect(() => {
		if (chart) {
			chart.data = data;
			chart.update('none');
		}
	});
</script>

<article data-theme="light">
	<div class="chart">
		<canvas bind:this={canvas}></canvas>
	</div>
</article>

<style>
	.chart {
		height: 400px;
	}
</style>