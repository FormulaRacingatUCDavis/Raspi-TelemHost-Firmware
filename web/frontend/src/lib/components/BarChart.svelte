<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';

	let { labels, title=null, datasets } = $props();

	let canvas: HTMLCanvasElement | null = $state(null);
	let chart: Chart | null = null;

	onMount(() => {
		if (canvas) {
			chart = new Chart(canvas, {
				type: 'bar',
				data: {
					labels: labels,
					datasets: datasets
				},
				options: {
					responsive: true,
					maintainAspectRatio: false,
					plugins: {
						title: {
							display: title != null,
							text: title
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
			chart.data.labels = labels;
			chart.data.datasets = datasets;
			chart.update();
		}
	});
</script>

<article data-theme="light" class="chart-container">
	<canvas bind:this={canvas}></canvas>
</article>

<style>
	.chart-container {
		height: 400px;
	}
</style>
