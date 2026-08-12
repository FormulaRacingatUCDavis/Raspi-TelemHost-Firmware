<script lang="ts">
	let file: File | null = null;

	async function uploadDBC(file: File) {
		const form = new FormData();
		form.append("file", file);

		const res = await fetch("/api/dbc/upload", {
			method: "PUT",
			body: form
		});

		const data = await res.json();
		console.log(data);
	}
</script>

<label>
	CAN DBC
	<input
		type="file"
		onchange={async (e) => {
			file = (e.currentTarget as HTMLInputElement).files?.[0] ?? null;

			if (file) {
				await uploadDBC(file);
			}
		}}
	/>
</label>

{#if file}
	<article data-theme="light">
		<details>
			<!-- svelte-ignore a11y_no_redundant_roles -->
			<summary role="button" class="outline"></summary>
			<article>
				<details>
					<!-- svelte-ignore a11y_no_redundant_roles -->
					<summary role="button" class="outline secondary">
						<!-- Placeholder -->
					</summary>
					<fieldset>
						<label>
							<input type="checkbox" name="english" />
							<!-- Placeholder -->
						</label>
					</fieldset>
				</details>
			</article>
		</details>
	</article>
{/if}
<input type="submit" disabled={!file}/>