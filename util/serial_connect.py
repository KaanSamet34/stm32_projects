import os
import argparse


def serial_connect(baud, databits=None, flow_control=None, parity=None, stopbits=None):

    # Default Command
    command = "tio --baudrate " + f"{baud} " + "--local-echo -a latest "

    # Additional Arguments
    if databits:
        command += "--databits " + f"{databits} "
    if flow_control:
        command += "--flow " + f"{flow_control} "
    if parity:
        command += "--parity " + f"{parity} "
    if stopbits:
        command += "--stopbits " + f"{stopbits} "

    print(command)
    os.system(command)


parser = argparse.ArgumentParser(description="Script that adds 3 numbers from CMD")
parser.add_argument("--baud", required=True, type=int)
parser.add_argument("--databits", required=False, type=int)
parser.add_argument("--flow_control", required=False, type=str)
parser.add_argument("--parity", required=False, type=str)
parser.add_argument("--stopbits", required=False, type=int)

args = parser.parse_args()

baud = args.baud
databits = args.databits
flow_control = args.flow_control
parity = args.parity
stopbits = args.stopbits

serial_connect(baud, databits, flow_control, parity, stopbits)
