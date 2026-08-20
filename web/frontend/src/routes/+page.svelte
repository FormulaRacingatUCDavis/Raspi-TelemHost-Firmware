<script lang="ts">
	import { onMount } from 'svelte';
	import BarChart from '$lib/components/BarChart.svelte';
	import LineChart from '$lib/components/LineChart.svelte';
	import mqtt from 'mqtt';

	let selectedSignals: string[] = [];

	onMount(async () => {
		try {
			const res = await fetch('/api/telemetry/config');
			const config = await res.json();

			selectedSignals = config.signals ?? [];

			const mqttClient = mqtt.connect('ws://localhost:9001');

			mqttClient.on('connect', () => {
				console.log('[MQTT] Connected to broker.');
			});

			mqttClient.on('close', () => {
				console.log('[MQTT] Connection closed.');
			});
		} catch (error) {
			console.error('Failed to fetch telemetry config:', error);
		}
	});
</script>

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
					<tr>
						<th scope="col">Source</th>
						<th scope="col">Message</th>
						<th scope="col">Signal</th>
						<th scope="col">Value</th>
					</tr>
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

<div>
	<div class="grid">
		<div>
			<small>Bar 1</small>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset>
							{#each selectedSignals as signal}
								<label>
									<input type="checkbox" name="graph1" />
									{signal}
								</label>
							{/each}
						</fieldset>
					</small>
				</div>
			</article>
			<BarChart />
		</div>
		<div>
			<small>Bar 2</small>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset>
							{#each selectedSignals as signal}
								<label>
									<input type="checkbox" name="graph1" />
									{signal}
								</label>
							{/each}
						</fieldset>
					</small>
				</div>
			</article>
			<BarChart />
		</div>
	</div>
</div>

<div>
	<div class="grid">
		<div>
			<small>Y1, X1</small>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset>
							{#each selectedSignals as signal}
								<label>
									<input type="radio" name="graph1" />
									{signal}
								</label>
							{/each}
						</fieldset>
					</small>
				</div>
			</article>
			<article>
				<small>Select Y-axis</small>
			</article>
		</div>
		<div>
			<small>Y2, X2</small>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset>
							{#each selectedSignals as signal}
								<label>
									<input type="radio" name="graph1" />
									{signal}
								</label>
							{/each}
						</fieldset>
					</small>
				</div>
			</article>
			<article>
				<small>Select Y-axis</small>
			</article>
		</div>
	</div>
</div>
<div class="grid">
	<LineChart />
	<LineChart />
</div>
