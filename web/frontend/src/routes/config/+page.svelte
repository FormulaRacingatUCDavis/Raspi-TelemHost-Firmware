<script lang="ts">
	import { onMount } from 'svelte';

	type Message = {
		id: number;
		signals: string[];
	};

	type Messages = Record<string, Message>;
	type DBCMessages = Record<string, Messages>;

	let file: File | null = null;
	let dbcFiles: string[] = [];
	let messagesData: Record<string, DBCMessages> = {};
	let selectedSignals = new Set<string>();

	function signalKey(dbc: string, sender: string, message: string, signal: string) {
		return `${dbc}|${sender}|${message}|${signal}`;
	}

	function isMessageChecked(dbc: string, sender: string, msgName: string, message: Message) {
		return message.signals.some((signal) =>
			selectedSignals.has(signalKey(dbc, sender, msgName, signal))
		);
	}

	function isSenderChecked(dbc: string, sender: string, messages: Messages) {
		return Object.entries(messages).some(([msgName, message]) =>
			isMessageChecked(dbc, sender, msgName, message)
		);
	}

	function toggleSignal(key: string, checked: boolean) {
		if (checked) {
			selectedSignals.add(key);
		} else {
			selectedSignals.delete(key);
		}

		selectedSignals = new Set(selectedSignals);
	}

	async function uploadDBC(file: File) {
		const form = new FormData();
		form.append('file', file);

		const res = await fetch('/api/dbc/upload', {
			method: 'PUT',
			body: form
		});

		if (res.ok) {
			await loadDBCs();
		}
	}

	async function updateConfig() {
		const dbc: Record<string, DBCMessages> = {};

		for (const key of selectedSignals) {
			const [dbcName, sender, msgName, signal] = key.split('|');
			const message = messagesData[dbcName]?.[sender]?.[msgName];

			if (!message) continue;

			dbc[dbcName] ??= {};
			dbc[dbcName][sender] ??= {};
			dbc[dbcName][sender][msgName] ??= {
				id: message.id,
				signals: []
			};

			dbc[dbcName][sender][msgName].signals.push(signal);
		}

		const res = await fetch('/api/config', {
			method: 'PUT',
			headers: {
				'Content-Type': 'application/json'
			},
			body: JSON.stringify({ dbc })
		});

		if (!res.ok) {
			console.error('Failed to update telemetry config');
		}
	}

	async function loadDBCs() {
		const res = await fetch('/api/dbc/list');
		dbcFiles = await res.json();

		messagesData = {};

		for (const dbcFile of dbcFiles) {
			const res = await fetch(`/api/dbc/messages?file=${encodeURIComponent(dbcFile)}`);

			const data = await res.json();
			messagesData[dbcFile] = data[dbcFile];
		}

		messagesData = { ...messagesData };
	}

	onMount(async () => {
		try {
			const res = await fetch('/api/config');
			const config = await res.json();

			for (const [dbc, senders] of Object.entries(config.dbc ?? {})) {
				for (const [sender, messages] of Object.entries(senders as DBCMessages)) {
					for (const [msgName, message] of Object.entries(messages)) {
						for (const signal of message.signals) {
							selectedSignals.add(signalKey(dbc, sender, msgName, signal));
						}
					}
				}
			}

			selectedSignals = new Set(selectedSignals);

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
			file = e.currentTarget.files?.[0] ?? null;

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
						<summary
							role="button"
							class={isSenderChecked(dbcFile, sender, messages) ? '' : 'outline'}
						>
							{sender}
						</summary>

						<article>
							{#each Object.entries(messages) as [msgName, message]}
								<details>
									<!-- svelte-ignore a11y_no_redundant_roles -->
									<summary
										role="button"
										class={isMessageChecked(dbcFile, sender, msgName, message)
											? 'secondary'
											: 'outline secondary'}
									>
										{msgName}
									</summary>

									<fieldset>
										{#each message.signals as signal}
											{@const key = signalKey(dbcFile, sender, msgName, signal)}

											<label>
												<input
													type="checkbox"
													checked={selectedSignals.has(key)}
													onchange={(e) => toggleSignal(key, e.currentTarget.checked)}
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

<input type="submit" value="Save Config" onclick={updateConfig} />
