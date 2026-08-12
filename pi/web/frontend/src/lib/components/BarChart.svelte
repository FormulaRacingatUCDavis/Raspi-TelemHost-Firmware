<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';

	let canvas: HTMLCanvasElement | null = $state(null);
	let chart: Chart | null = null;

	onMount(() => {
		if (canvas) {
			chart = new Chart(canvas, {
				type: 'bar',
				data: {
					labels: [],
					datasets: [
						{
							data: []
						}
					]
				},
				options: {
					responsive: true,
					maintainAspectRatio: false
				}
			});
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