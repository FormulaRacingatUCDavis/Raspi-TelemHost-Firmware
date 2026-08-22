<script lang="ts">
	import { onMount } from 'svelte';
	import BarChart from '$lib/components/BarChart.svelte';
	import LineChart from '$lib/components/LineChart.svelte';
	import mqtt from 'mqtt';

	let liveData: Record<string, number | string> = {};
	let mcState = '--';

	let cellTemps: number[][] = Array.from({ length: 16 }, () => Array(4).fill(0));
	let signalData: Record<string, { x: number; y: number }[]> = {};

	let selectedSignal1 = 'None selected';
	let selectedSignal2 = 'None selected';

	const liveValues = [
		{ id: 0x766, signals: ['dashboard_state'] },
		{ id: 0x380, signals: ['pei_bms_status'] },
		{ id: 0xab, signals: ['inv_run_fault', 'inv_post_fault'] },
		{ id: 0xc0, signals: ['dashboard_torque'] },
		{ id: 0xa0, signals: ['inv_module_a_temp', 'inv_module_b_temp', 'inv_module_c_temp'] },
		{ id: 0xa5, signals: ['inv_motor_speed'] },
		{ id: 0x381, signals: ['pei_soc', 'pei_pack_voltage'] },
		{ id: 0x384, signals: ['pei_subpack', 'pei_group', 'pei_temp_1', 'pei_temp_2', 'pei_temp_3'] },
		{ id: 0x388, signals: ['pei_current'] },
		{ id: 0x504, signals: ['motor_power', 'acc_power'] },
		{ id: 0x400, signals: ['inlet_water_temp', 'outlet_water_temp'] }
	];

	const signals = liveValues.flatMap((value) => value.signals);

	onMount(() => {
		const mqttClient = mqtt.connect('ws://localhost:9001');

		mqttClient.on('connect', () => {
			console.log('[MQTT] Connected to broker.');
			mqttClient.subscribe('can/frame');
			mqttClient.subscribe('can/log/status');
		});

		mqttClient.on('message', (topic, message) => {
			if (topic === 'can/frame') {
				try {
					const data = JSON.parse(message.toString());
					const liveValue = liveValues.find((value) => value.id === data.id);

					if (liveValue) {
						for (const signal of liveValue.signals) {
							if (signal in data) {
								liveData = { ...liveData, [signal]: data[signal] };

								if (typeof data[signal] === 'number' && data.timestamp !== undefined) {
									signalData = {
										...signalData,
										[signal]: [
											...(signalData[signal] ?? []),
											{
												x: Number(data.timestamp),
												y: Number(data[signal])
											}
										].slice(-100)
									};
								}
							}
						}

						if (data.id === 0xab) {
							mcState = data.inv_run_fault !== 'Normal' ? data.inv_run_fault : data.inv_post_fault;
						}

						if (data.id === 0x384) {
							const subpack = Number(data.pei_subpack);
							const group = Number(data.pei_group);

							if (subpack >= 0 && subpack < 4 && group >= 0 && group < 6) {
								const startCell = group * 3;

								cellTemps[startCell][subpack] = Number(data.pei_temp_1);

								if (group < 5) {
									cellTemps[startCell + 1][subpack] = Number(data.pei_temp_2);
									cellTemps[startCell + 2][subpack] = Number(data.pei_temp_3);
								}

								cellTemps = [...cellTemps];
							}
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
		</table>

		<table class="striped">
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

<div class="grid">
	<div class="grid">
		<BarChart
			data={{
				labels: ['A', 'B', 'C'],
				datasets: [
					{
						label: 'Modules [C]',
						data: [
							liveData.inv_module_a_temp ?? 0,
							liveData.inv_module_b_temp ?? 0,
							liveData.inv_module_c_temp ?? 0
						]
					}
				]
			}}
		/>

		<BarChart
			data={{
				labels: ['Inlet', 'Outlet'],
				datasets: [
					{
						label: 'Cooling [C]',
						data: [liveData.inlet_water_temp ?? 0, liveData.outlet_water_temp ?? 0]
					}
				]
			}}
		/>
	</div>

	<BarChart
		title="Cell Temps [C]"
		data={{
			labels: ['1', '2', '3', '4'],
			datasets: [
				{
					label: 'A',
					data: cellTemps[0]
				},
				{
					label: 'B',
					data: cellTemps[3]
				},
				{
					label: 'C',
					data: cellTemps[6]
				},
				{
					label: 'D',
					data: cellTemps[9]
				},
				{
					label: 'E',
					data: cellTemps[12]
				},
				{
					label: 'F',
					data: cellTemps[15]
				}
			]
		}}
	/>
</div>

<div>
	<div class="grid">
		<div>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset>
							{#each signals as signal}
								<label>
									<input type="radio" name="signal1" value={signal} bind:group={selectedSignal1} />
									{signal}
								</label>
							{/each}
						</fieldset>
					</small>
				</div>
			</article>
		</div>

		<div>
			<article>
				<div class="overflow-auto" style="max-height: 100px;">
					<small>
						<fieldset>
							{#each signals as signal}
								<label>
									<input type="radio" name="signal2" value={signal} bind:group={selectedSignal2} />
									{signal}
								</label>
							{/each}
						</fieldset>
					</small>
				</div>
			</article>
		</div>
	</div>
</div>

<div class="grid">
	<LineChart
		data={{
			datasets: [
				{
					label: selectedSignal1,
					data: signalData[selectedSignal1] ?? [],
					borderWidth: 1,
					pointRadius: 0,
					pointHitRadius: 0,
					tension: 0,
					fill: false,
					spanGaps: false
				}
			]
		}}
	/>

	<LineChart
		data={{
			datasets: [
				{
					label: selectedSignal2,
					data: signalData[selectedSignal2] ?? [],
					borderWidth: 1,
					pointRadius: 0,
					pointHitRadius: 0,
					tension: 0,
					fill: false,
					spanGaps: false
				}
			]
		}}
	/>
</div>
