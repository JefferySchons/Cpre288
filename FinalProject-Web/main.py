from flask import Flask, render_template, request, jsonify
import sys
import serial

sys.path.append('PyRobot')
from command import *
from status import *
from receiver import *
from sender import *

app = Flask(__name__)
global myreceiver
global mysender

@app.route('/', methods=['POST', 'GET'])
def control_center():
  return render_template('control_center.html', **locals())

@app.route('/api/getstatus')
def get_status():
    statuses = []
    myreceiver.receive() #Poll for data on serial port or FIFO
    while myreceiver.isNewStatusAvailable():
        s = myreceiver.getStatus()
        if not s.isDataStatus:
            statuses.append({
                             'type' : 'status',
                             'id' : s.command.command_id,
                             'result' : s.actualResultString(),
                             'code' : s.abort_reason,
                             'string' : s.abortReasonString(),
                             })
        elif isinstance(s, ScanDataStatus):
            statuses.append({
                             'type' : 'scan_result',
                             'id' : s.command.command_id,
                             'angle' : s.angle,
                             'distance' : s.distance,
                             'size' : s.size
                             })
        elif isinstance(s, PollSensorStatus):
            statuses.append({
                             'type' : 'sensor_value',
                             'sensor' : s.sensor,
                             'value' : s.value,
                            })
    return jsonify(statuses=statuses) if statuses else "{}"

#TODO: Parameters and stuff
@app.route('/api/moveforward', methods=['POST'])
def move_foward():
    if request.method == 'POST':
        distance = num(request.data.split("=")[1])
        distance = distance if distance and distance > 0 and distance < 5000 else 200
        mysender.sendCommand(MoveForwardCommand(distance))
    return ''

@app.route('/api/movereverse', methods=['POST'])
def move_reverse():
    if request.method == 'POST':
        distance = num(request.data.split("=")[1])
        distance = distance if distance and distance > 0 and distance < 5000 else 200
        mysender.sendCommand(MoveReverseCommand(distance))
    return ''

@app.route('/api/rotateclockwise', methods=['POST'])
def rotate_clockwise():
    if request.method == 'POST':
        degrees = num(request.data.split("=")[1])
        degrees = degrees if degrees and degrees > 0 and degrees <= 360 else 90
        mysender.sendCommand(RotateClockwiseCommand(degrees))
    return ''

@app.route('/api/rotatecounterclockwise', methods=['POST'])
def rotate_counterclockwise():
    if request.method == 'POST':
        degrees = num(request.data.split("=")[1])
        degrees = degrees if degrees and degrees > 0 and degrees <= 360 else 90
        mysender.sendCommand(RotateCounterclockwiseCommand(degrees))
    return ''

@app.route('/api/sendscan', methods=['POST'])
def begin_scan():
    if request.method == 'POST':
        max_distance = num(request.data.split("=")[1])
        max_distance = max_distance if max_distance > 0 and max_distance < 250 else 100
        mysender.sendCommand(BeginScanCommand(max_distance))
    return ''

@app.route('/api/playmusic/<song_id>', methods=['POST'])
def play_music(song_id):
    if request.method == 'POST':
        mysender.sendCommand(PlayMusicCommand(song_id))
    return ''

@app.route('/api/pollsensors', methods=['POST'])
def poll_sensors():
    if request.method == 'POST':
        mysender.sendCommand(PollSensorCommand())
    return ''


def num(s):
    try:
        return int(s)
    except ValueError:
        return None

if __name__ == '__main__':
  if 'debug' in sys.argv:
     mysender = DebugSender()
     myreceiver = FifoReceiver(mysender, 'tempfifo')
  else:
     ser = serial.Serial('/dev/rfcomm0', 38400, timeout=0, writeTimeout=0)
     mysender = PySerialSender(ser)
     myreceiver = PySerialReceiver(mysender, ser)
  app.debug = True
  app.run(host='0.0.0.0')
