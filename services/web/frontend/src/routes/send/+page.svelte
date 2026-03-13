<script>
</script>

<article data-theme="light">
	<select>
		<option selected disabled value="">Select</option>
		<option value="traction-control">Traction Control</option>
	</select>
	<hr />
	<small>
		<div class="overflow-auto" style="max-height: 300px;">
			<table>
				<thead>
					<tr>
						<th>Topic</th>
						<th>Description</th>
					</tr>
				</thead>
				<tbody>
					<tr>
						<th scope="row">Traction Control</th>
						<td>Update traction control parameters</td>
					</tr>
				</tbody>
			</table>
		</div>
	</small>
</article>
<article data-theme="light">
    <h5>Traction Control</h5>
    <hr />
	<input type="number" name="Kp" placeholder="Kp" aria-label="Number" />
	<input type="number" name="Ki" placeholder="Ki" aria-label="Number" />
	<input type="number" name="Kd" placeholder="Kd" aria-label="Number" />
    <hr />
	<button>Update</button>
</article>
