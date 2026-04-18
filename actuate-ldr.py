# ******************************************************************************
#  PROJECT      : ESP32 LDR Live Plotter
#  DESCRIPTION  : Subscribes to the MQTT topic published by the ESP32,
#                 parses the JSON payload, and plots the LDR value in real
#                 time using Matplotlib's animation loop.
#
#  INSTALL      : pip install paho-mqtt matplotlib
#
#  USAGE        : python ldr_plotter.py
#                 Make sure broker address matches what you set in ESP32 code.
#
#  CODED BY     : SWAN LAB, IIT Kharagpur
# ******************************************************************************

import json
import threading
import time
from collections import deque

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import paho.mqtt.client as mqtt


# ── MQTT CONFIG ────────────────────────────────────────────────────────────── #

BROKER        = "broker.hivemq.com"
PORT          = 1883
TOPIC_DATA    = "swan/ldr/data"
DARK_THRESHOLD = 15   # Mirror the value set in ESP32 — for reference line

# ── PLOT CONFIG ────────────────────────────────────────────────────────────── #

MAX_POINTS    = 100     # How many data points to keep on screen at once
UPDATE_MS     = 200     # Graph refresh interval in milliseconds


# ── SHARED DATA (thread-safe deque) ───────────────────────────────────────── #

ldr_values  = deque(maxlen=MAX_POINTS)
time_stamps = deque(maxlen=MAX_POINTS)
states      = deque(maxlen=MAX_POINTS)   # "DARK" or "LIGHT"
start_time  = time.time()
latest_state = "—"


# ── MQTT CALLBACKS ─────────────────────────────────────────────────────────── #

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"[MQTT] Connected to {BROKER}")
        client.subscribe(TOPIC_DATA)
        print(f"[MQTT] Subscribed to {TOPIC_DATA}")
    else:
        print(f"[MQTT] Connection failed (rc={rc})")


def on_message(client, userdata, msg):
    global latest_state
    try:
        data = json.loads(msg.payload.decode())
        elapsed = time.time() - start_time
        ldr_values.append(data["ldr"])
        time_stamps.append(elapsed)
        states.append(data.get("state", "—"))
        latest_state = data.get("state", "—")
    except (json.JSONDecodeError, KeyError) as e:
        print(f"[WARN] Bad payload: {e}")


# ── START MQTT IN BACKGROUND THREAD ───────────────────────────────────────── #

client = mqtt.Client(client_id="SWAN_LAB_Plotter")
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT, keepalive=60)

mqtt_thread = threading.Thread(target=client.loop_forever, daemon=True)
mqtt_thread.start()


# ── MATPLOTLIB LIVE PLOT ───────────────────────────────────────────────────── #

fig, ax = plt.subplots(figsize=(10, 5))
fig.patch.set_facecolor("#1a1a2e")
ax.set_facecolor("#16213e")

line_ldr,  = ax.plot([], [], color="#00d4ff", linewidth=1.8, label="LDR value")
line_thr   = ax.axhline(y=DARK_THRESHOLD, color="#ff6b6b",
                         linewidth=1.2, linestyle="--", label=f"Threshold ({DARK_THRESHOLD})")

ax.set_xlim(0, MAX_POINTS * 0.5)   # x = seconds, grows with data
ax.set_ylim(0, 700)
ax.set_xlabel("Time (s)", color="#aaaaaa")
ax.set_ylabel("LDR value (0–4095)", color="#aaaaaa")
ax.set_title("SWAN LAB — ESP32 LDR Live Monitor", color="#ffffff", fontsize=13)
ax.tick_params(colors="#aaaaaa")
ax.legend(loc="upper right", facecolor="#1a1a2e", labelcolor="#ffffff")
ax.grid(True, color="#2a2a4a", linewidth=0.5)

# State text in top-left corner
state_text = ax.text(0.02, 0.92, "", transform=ax.transAxes,
                     fontsize=12, color="#ffffff",
                     bbox=dict(boxstyle="round,pad=0.3", facecolor="#0f3460", alpha=0.8))


def animate(frame):
    if len(time_stamps) < 2:
        return line_ldr, state_text

    xs = list(time_stamps)
    ys = list(ldr_values)

    line_ldr.set_data(xs, ys)

    # Auto-scroll x axis
    ax.set_xlim(max(0, xs[-1] - MAX_POINTS * 0.5), xs[-1] + 1)

    # Colour background based on state
    colour = "#1a0a0a" if latest_state == "DARK" else "#0a1a0a"
    ax.set_facecolor(colour)

    state_text.set_text(f"State: {latest_state}")

    return line_ldr, state_text


ani = animation.FuncAnimation(
    fig, animate, interval=UPDATE_MS, blit=False, cache_frame_data=False
)

plt.tight_layout()
plt.show()
