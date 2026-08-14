<script lang="ts">
	import { onMount } from 'svelte';

	let file: File | null = null;
	let messagesData: Record<string, Record<string, string[]>> = {};
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

	function isMessageChecked(msgName: string, signals: string[]): boolean {
		return signals.some((signal) => selectedSignals.has(`${msgName}.${signal}`));
	}

	function isSenderChecked(messages: Record<string, string[]>): boolean {
		return Object.entries(messages).some(([msgName, signals]) =>
			isMessageChecked(msgName, signals)
		);
	}

	onMount(async () => {
		try {
			const [messagesRes, configRes] = await Promise.all([
				fetch('/api/dbc/messages'),
				fetch('/api/telemetry/config')
			]);

			messagesData = await messagesRes.json();

			const config = await configRes.json();
			selectedSignals = new Set(config.signals ?? []);
		} catch (error) {
			console.error('Failed to fetch messages:', error);
		}
	});
</script>

<input
	type="file"
	onchange={async (e) => {
		file = (e.currentTarget as HTMLInputElement).files?.[0] ?? null;

		if (file) {
			await uploadDBC(file);
		}
	}}
/>

{#if Object.keys(messagesData).length > 0}
	<article data-theme="light">
		{#each Object.entries(messagesData) as [sender, messages]}
			<details>
				<!-- svelte-ignore a11y_no_redundant_roles -->
				<summary role="button" class={isSenderChecked(messages) ? '' : 'outline'}>
					{sender}
				</summary>

				<article>
					{#each Object.entries(messages) as [msgName, signals]}
						<details>
							<!-- svelte-ignore a11y_no_redundant_roles -->
							<summary
								role="button"
								class={isMessageChecked(msgName, signals) ? 'secondary' : 'outline secondary'}
							>
								{msgName}
							</summary>

							<fieldset>
								{#each signals as signal}
									{@const signalId = `${msgName}.${signal}`}

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
{/if}

<input type="submit" onclick={updateConfig} />
