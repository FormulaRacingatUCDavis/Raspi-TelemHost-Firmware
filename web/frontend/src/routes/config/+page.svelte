<script lang="ts">
	import { onMount } from 'svelte';

	let file: File | null = null;
	let dbcFiles: string[] = [];
	let messagesData: Record<string, Record<string, Record<string, string[]>>> = {};
	let selectedSignals = new Set<string>();

	async function uploadDBC(file: File) {
		const form = new FormData();
		form.append('file', file);

		const res = await fetch('/api/dbc/upload', {
			method: 'PUT',
			body: form
		});

		const data = await res.json();
		console.log(data);

		if (res.ok) {
			await loadDBCs();
		}
	}

	async function updateConfig() {
		const res = await fetch('/api/telemetry/config', {
			method: 'PUT',
			headers: {
				'Content-Type': 'application/json'
			},
			body: JSON.stringify({
				signals: [...selectedSignals]
			})
		});

		if (!res.ok) {
			console.error('Failed to update telemetry config');
		}
	}

	function isMessageChecked(dbcName: string, msgName: string, signals: string[]): boolean {
		return signals.some((signal) => selectedSignals.has(`${dbcName}.${msgName}.${signal}`));
	}

	function isSenderChecked(dbcName: string, messages: Record<string, string[]>): boolean {
		return Object.entries(messages).some(([msgName, signals]) =>
			isMessageChecked(dbcName, msgName, signals)
		);
	}

	async function loadDBCs() {
		const res = await fetch('/api/dbc/list');
		dbcFiles = await res.json();

		messagesData = {};

		for (const dbcFile of dbcFiles) {
			const res = await fetch(`/api/dbc/messages?file=${encodeURIComponent(dbcFile)}`);
			const data = await res.json();

			messagesData[dbcFile] = data;
			messagesData = { ...messagesData };
		}
	}

	onMount(async () => {
		try {
			const configRes = await fetch('/api/telemetry/config');

			const config = await configRes.json();
			selectedSignals = new Set(config.signals ?? []);

			await loadDBCs();
		} catch (error) {
			console.error('Failed to fetch DBC data:', error);
		}
	});
</script>

<label>
	CAN DBC
	<input
		type="file"
		accept=".dbc"
		onchange={async (e) => {
			file = (e.currentTarget as HTMLInputElement).files?.[0] ?? null;

			if (file) {
				await uploadDBC(file);
			}
		}}
	/>
</label>

{#each dbcFiles as dbcFile}
	{#if messagesData[dbcFile]}
		<div>
			<small>{dbcFile}</small>

			<article data-theme="light">
				{#each Object.entries(messagesData[dbcFile]) as [sender, messages]}
					<details>
						<!-- svelte-ignore a11y_no_redundant_roles -->
						<summary role="button" class={isSenderChecked(dbcFile, messages) ? '' : 'outline'}>
							{sender}
						</summary>

						<article>
							{#each Object.entries(messages) as [msgName, signals]}
								<details>
									<!-- svelte-ignore a11y_no_redundant_roles -->
									<summary
										role="button"
										class={isMessageChecked(dbcFile, msgName, signals)
											? 'secondary'
											: 'outline secondary'}
									>
										{msgName}
									</summary>

									<fieldset>
										{#each signals as signal}
											{@const signalId = `${dbcFile}.${msgName}.${signal}`}

											<label>
												<input
													type="checkbox"
													checked={selectedSignals.has(signalId)}
													onchange={(e) => {
														const checked = (e.currentTarget as HTMLInputElement).checked;

														if (checked) {
															selectedSignals.add(signalId);
														} else {
															selectedSignals.delete(signalId);
														}

														selectedSignals = new Set(selectedSignals);
													}}
												/>
												{signal}
											</label>
										{/each}
									</fieldset>
								</details>
							{/each}
						</article>
					</details>
				{/each}
			</article>
		</div>
	{/if}
{/each}

<input type="submit" onclick={updateConfig} />
