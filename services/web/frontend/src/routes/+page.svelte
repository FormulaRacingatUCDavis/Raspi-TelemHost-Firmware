<script lang="ts">
	import { onMount } from 'svelte';
	import BarChart from '$lib/components/BarChart.svelte';
	import LineChart from '$lib/components/LineChart.svelte';

	onMount(() => {
		return () => {};
	});
</script>

<div class="grid">
	<div>
		<form>
			<fieldset class="grid">
				<input placeholder="Enter driver name (optional)" />
				<div class="grid">
					<input type="submit" value="Record" />
					<input type="submit" class="secondary" value="Stop" disabled />
				</div>
			</fieldset>
		</form>
		<article data-theme="light">
			<small>
				<table class="striped">
					<thead>
						<tr><th scope="col">Source</th><th scope="col">State</th></tr>
					</thead>
					<tbody>
						<tr>
							<th>
								<!-- Placeholder -->
							</th>
						</tr>
					</tbody>
					<thead>
						<tr><th scope="col">Metric</th><th scope="col">Value</th></tr>
					</thead>
					<tbody>
						<tr>
							<th>
								<!-- Placeholder -->
							</th>
						</tr>
					</tbody>
				</table>
			</small>
		</article>
	</div>
	<div class="grid">
		<BarChart />
		<BarChart />
	</div>
</div>

<div class="grid">
	<article>
		<small>Graph 1</small>
		<hr />
		<div class="overflow-auto" style="max-height: 75px;">
			<small>
				<fieldset>
					<label>
						<input type="radio" name="graph1" />
						<!-- Placeholder -->
					</label>
				</fieldset>
			</small>
		</div>
	</article>
	<article>
		<small>Graph 2</small>
		<hr />
		<div class="overflow-auto" style="max-height: 75px;">
			<small>
				<fieldset>
					<label>
						<input type="radio" name="graph2" />
						<!-- Placeholder -->
					</label>
				</fieldset>
			</small>
		</div>
	</article>
</div>

<div class="grid">
	<LineChart />
	<LineChart />
</div>
