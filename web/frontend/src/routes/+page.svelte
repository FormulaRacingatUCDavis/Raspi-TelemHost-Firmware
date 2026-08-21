<script lang="ts">
	import { onMount } from 'svelte';
	import BarChart from '$lib/components/BarChart.svelte';
	import LineChart from '$lib/components/LineChart.svelte';
	import mqtt from 'mqtt';

	let liveData: Record<string, number | string> = {};
	let mcState = '--';

	const liveValues = [
		{ id: 0x766, signals: ['dashboard_state'] },
		{ id: 0x380, signals: ['pei_bms_status'] },
		{ id: 0xab, signals: ['inv_run_fault', 'inv_post_fault'] },
		{ id: 0xc0, signals: ['dashboard_torque'] },
		{ id: 0xa5, signals: ['inv_motor_speed'] },
		{ id: 0x381, signals: ['pei_soc'] },
		{ id: 0x503, signals: ['motor_power', 'acc_power'] },
		{ id: 0x400, signals: ['inlet_water_temp', 'outlet_water_temp'] }
	];

	onMount(() => {
		const mqttClient = mqtt.connect('ws://localhost:9001');

		mqttClient.on('connect', () => {
			console.log('[MQTT] Connected to broker.');
			mqttClient.subscribe('can/frame');
			mqttClient.subscribe('can/log/status');
		});

		mqttClient.on('disconnect', () => {
			console.log('[MQTT] Disconnected from broker.');
		});

		mqttClient.on('reconnect', () => {
			console.log('[MQTT] Reconnecting to broker...');
		});

		mqttClient.on('error', (error) => {
			console.error('[MQTT] Error:', error);
		});

		mqttClient.on('message', (topic, message) => {
			if (topic === 'can/frame') {
				try {
					const data = JSON.parse(message.toString());
					console.log('[MQTT] Received:', data);
					const liveValue = liveValues.find((value) => value.id === data.id);

					if (liveValue) {
						for (const signal of liveValue.signals) {
							if (signal in data) {
								liveData = { ...liveData, [signal]: data[signal] };
							}
						}

						if (data.id === 0xab) {
							mcState = data.inv_run_fault !== 'Normal' ? data.inv_run_fault : data.inv_post_fault;
						}
					}
				} catch (error) {
					console.error('Failed to parse CAN frame:', error);
				}
			}
		});

		return () => {
			mqttClient.end();
		};
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
						<th scope="col">Status</th>
					</tr>
				</thead>
				<tbody>
					<tr>
						<th scope="row">VCU</th>
						<td>{liveData.dashboard_state ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">PEI</th>
						<td>{liveData.pei_bms_status ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">MC</th>
						<td>{mcState}</td>
					</tr>
				</tbody>
				<thead>
					<tr>
						<th scope="col">Source</th>
						<th scope="col">Metric</th>
						<th scope="col">Value</th>
					</tr>
				</thead>
				<tbody>
					<tr>
						<th scope="row">VCU</th>
						<th scope="row">MC Power [W]</th>
						<td>{liveData.motor_power ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">VCU</th>
						<th scope="row">AC Power [W]</th>
						<td>{liveData.acc_power ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">PEI</th>
						<th scope="row">Current [A]</th>
						<td>{liveData.pei_current ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">PEI</th>
						<th scope="row">Voltage [V]</th>
						<td>{liveData.pei_pack_voltage ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">PEI</th>
						<th scope="row">SOC [%]</th>
						<td>{liveData.pei_soc ?? '--'}</td>
					</tr>
					<tr>
						<th scope="row">Xsens</th>
						<th scope="row">Latitude [°]</th>
						<td><!-- Placeholder --></td>
					</tr>
					<tr>
						<th scope="row">Xsens</th>
						<th scope="row">Longitude [°]</th>
						<td><!-- Placeholder --></td>
					</tr>
				</tbody>
			</table>
		</small>
	</article>
</div>

<div class="grid">
	<div class="grid">
		<BarChart
			labels={['A', 'B', 'C']}
			datasets={[
				{
					label: 'Modules [C]',
					data: [
						liveData.inv_module_a_temp ?? 0,
						liveData.inv_module_b_temp ?? 0,
						liveData.inv_module_c_temp ?? 0
					]
				}
			]}
		/>
		<BarChart
			labels={['Inlet', 'Outlet']}
			datasets={[
				{
					label: 'Cooling [C]',
					data: [liveData.inlet_water_temp ?? 0, liveData.outlet_water_temp ?? 0]
				}
			]}
		/>
	</div>
	<BarChart
		labels={['1', '2', '3', '4']}
		title='Cell Temps [C]'
		datasets={[
			{
				label: 'A',
				data: [42, 47, 51, 49]
			},
			{
				label: 'B',
				data: [38, 44, 48, 52]
			},
			{
				label: 'C',
				data: [55, 51, 49, 47]
			},
			{
				label: 'D',
				data: [46, 43, 50, 48]
			},
			{
				label: 'E',
				data: [46, 43, 50, 48]
			},
			{
				label: 'F',
				data: [46, 43, 50, 48]
			}
		]}
	/>
</div>

<div>
	<div class="grid">
		<div>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset></fieldset>
					</small>
				</div>
			</article>
		</div>
		<div>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset></fieldset>
					</small>
				</div>
			</article>
		</div>
	</div>
</div>

<div class="grid">
	<LineChart />
	<LineChart />
</div>
