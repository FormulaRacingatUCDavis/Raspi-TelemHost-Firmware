<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';

	let { title = null, data } = $props();

	let canvas: HTMLCanvasElement | null = $state(null);
	let chart: Chart | null = null;

	onMount(() => {
		if (canvas) {
			chart = new Chart(canvas, {
				type: 'bar',
				data: data,
				options: {
					responsive: true,
					maintainAspectRatio: false,
					animation: false,
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
			chart.data = data;
			chart.update('none');
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
