import subprocess
from nicegui import app, ui, run

# FRUCD Brand 2.0 Colors
FRUCD_DARK_BLUE = '#003a70'

app.add_static_files('/static', 'static')


def online_check(ip):
    ping_out = subprocess.run(f"ping -c 1 -W 2 {ip}", shell=True, capture_output=True, text=True).stdout
    if "1 received" in ping_out:
        return True
    return False


def frucd_repeat_background():
    ui.add_head_html("<style>body {background-image: url('/static/FRUCD_logo.png'); background-size: 5%;}</style>")


@ui.page('/connected_devices')
def connected_devices():
    frucd_repeat_background()

    columns = [
                {'name': 'label', 'label': 'Label', 'field': 'label', 'required': True, 'align': 'left', 'sortable': True},
                {'name': 'address', 'label': 'Address', 'field': 'address', 'required': True, 'sortable': True},
                {'name': 'status', 'label': 'Status', 'field': 'status', 'required': True, 'sortable': True},
            ]
    rows = [
        {'label': 'Oracle Cloud Server', 'address': "10.66.66.1", "status": "Connected"},
        {'label': 'Ubiquiti Mobile Router', 'address': "10.66.66.3"},
        {'label': 'Rasp Pi Dash', 'address': "192.168.105.6"},
        {'label': 'TelemHost', 'address': "192.168.105.3"},
    ]

    with ui.card(align_items='center').classes('fixed-center'):
        # TODO: MAKE TABLE WITH LIVE INFO OF CONNECTED DEVICES
        device_table = ui.table(columns=columns, rows=rows, row_key='name', pagination=7)

    async def update_device_table():
        for device in device_table.rows:
            is_online = await run.io_bound(online_check, device["address"])
            if is_online:
                device["status"] = "Connected"
            else:
                device["status"] = "Offline"

        device_table.update()

    ui.timer(1, update_device_table)


@ui.page('/')
def home_page():
    frucd_repeat_background()

    with ui.card(align_items='center').classes('fixed-center'):
        ui.image('/static/FRUCD_GD_White(1).png')
        with ui.row():
            ui.button('Telemetry Web GUI', on_click=lambda: ui.navigate.to('http://0.0.0.0', new_tab=True)).classes(f'!bg-[{FRUCD_DARK_BLUE}]')
            ui.button('Connected Devices', on_click=lambda: ui.navigate.to('/connected_devices', new_tab=False)).classes(f'!bg-[{FRUCD_DARK_BLUE}]')


ui.run(port=8000, show=False, reload=False)
