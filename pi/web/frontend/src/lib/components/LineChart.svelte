<script lang="ts">
	import { onMount } from 'svelte';
	import Chart from 'chart.js/auto';

	let canvas: HTMLCanvasElement | null = $state(null);
	let chart: Chart | null = null;

	onMount(() => {
		if (canvas) {
			chart = new Chart(canvas, {
				type: 'line',
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
