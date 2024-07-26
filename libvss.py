import asyncio
import json
import logging
import signal

from sdv.vdb.reply import DataPointReply
from sdv.vehicle_app import VehicleApp
#from Vehicle import Vehicle, vehicle  # type: ignore
from sdv_model import Vehicle, vehicle  # type: ignore

def synchronize_async_helper(to_await):
    async_response = []

    async def run_and_capture_result():
        r = await to_await
        async_response.append(r)

    loop = asyncio.get_event_loop()
    coroutine = run_and_capture_result()
    loop.run_until_complete(coroutine)
    return async_response[0]

def get_accel_pedal_sts():
    accelPedalPos = synchronize_async_helper(vehicle.Chassis.Accelerator.PedalPosition.get())
    return accelPedalPos.value
