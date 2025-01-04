import {FormEvent, useEffect, useState} from 'react'
// import reactLogo from './assets/react.svg'
// import viteLogo from '/vite.svg'
import './App.css'
import {ITemperatureSlot, ITimeSlot} from './types';

function formatTime(time: Date) {
	return time.getUTCHours() + ':' + time.getUTCMinutes().toString().padStart(2, '0');
}

function parseTime(time: string) {
	const [hours, minutes] = time.split(':').map(Number);
	return (hours * 60 + minutes) * 60 * 1000;
}

export default function App() {
	//const [count, setCount] = useState(0);
	const ipAddress = 'http://192.168.1.187';

	const [timeSlots, setTimeSlots] = useState<ITimeSlot[]>([]);
	const [tempSlots, setTempSlots] = useState<number[]>([]);

	const [test, setTest] = useState<number>(0);

	// const [temperature, setTemperature] = useState<number>(21.0);
	// const [tempSlot, setTempSlot] = useState<number>(0);

	const fetchTempSlots = () => {
		fetch(`${ipAddress}/api/slots/temperature`, {
			method: 'GET',
			mode: 'cors',
		})
			.then(response => response.json())
			.then((data: number[]) => {
				setTempSlots(data);
			})
			.catch(console.error);
	}

	const fetchTimeSlots = () => {
		fetch(`${ipAddress}/api/slots/time`, {
			method: 'GET',
			mode: 'cors',
		})
			.then(response => response.json())
			.then((data: ITimeSlot[]) => {
				setTimeSlots(data);
			})
			.catch(console.error);
	}

	useEffect(() => {
		fetchTimeSlots();
	}, []);

	useEffect(() => {
		fetchTempSlots();
	}, []);

	const handleTimeSlotSubmit = (event: FormEvent<HTMLFormElement>) => {
		event.preventDefault();

		const formData = new FormData(event.target as HTMLFormElement);
		const {slot, tempSlot, from, to, active} = Object.fromEntries(formData);

		// console.log(slot, tempSlot, from, to, active);
		// console.log(Date.parse("1970-01-01T" + from.toString()), new Date(to.toString()));

		fetch(`${ipAddress}/api/slots/time`, {
			method: 'POST',
			mode: 'cors',
			headers: {
				'Content-Type': 'application/json',
			},
			body: JSON.stringify([{
				slot: parseInt(slot.toString()),
				temperatureSlot: parseInt(tempSlot.toString()),
				startTime: parseTime(from.toString()),
				endTime: parseTime(to.toString()),
				days: 0x7F,
				active: active == 'on' ? true : false
			}])
		})
			.then(res => res.json())
			.then((m) => {
				console.log(m);
				fetchTimeSlots();
			})
			.catch(console.error);
	};

	const handleTempSlotSubmit = (event: FormEvent<HTMLFormElement>) => {
		event.preventDefault();

		const formData = new FormData(event.target as HTMLFormElement);
		const {slot, temperature} = Object.fromEntries(formData);

		fetch(`${ipAddress}/api/slots/temperature`, {
			method: 'POST',
			mode: 'cors',
			headers: {
				'Content-Type': 'application/json',
			},
			body: JSON.stringify([{
				slot: slot,
				temperature: temperature
			}])
		})
			.then(res => res.json())
			.then((m) => {
				console.log(m);
				fetchTempSlots();
			})
			.catch(console.error);
	};

	return (
		<div style={{display: 'flex'}}>
			<div style={{display: 'flex', flexDirection: 'column', margin: '10px'}}>
				<b>Time slots:</b>
				{timeSlots.map((timeSlot, index) => (
					<div key={index} style={{
						border: '1px solid black',
						padding: '5px',
						background: '#444',
						textAlign: 'left'
					}}>
						<div>Slot: {index}</div>
						<div>Start: {formatTime(new Date(timeSlot.startTime))}</div>
						<div>End: {formatTime(new Date(timeSlot.endTime))}</div>
						<div>Days: {timeSlot.days}</div>
						<div>Temperature slot: {timeSlot.temperatureSlot === null ? 'null' : `${timeSlot.temperatureSlot} (${tempSlots[timeSlot.temperatureSlot]} °C)`}</div>
						<div>Enabled: {timeSlot.active ? 'true' : 'false'}</div>
					</div>
				))}
			</div>
			<div style={{display: 'flex', flexDirection: 'column', margin: '10px'}}>
				<b>Temperature slots:</b>
				{tempSlots.map((slot, index) => (
					<div key={index} style={{
						border: '1px solid black',
						padding: '5px',
						background: '#444',
						textAlign: 'left'
					}}>
						<div>Slot: {index}</div>
						<div>Temperature: {slot} °C</div>
					</div>
				))}
			</div>
			<div style={{display: 'flex', flexDirection: 'column', margin: '10px'}}>
				<b>Set temperature slot:</b>
				<div style={{
					border: '1px solid black',
					padding: '5px',
					background: '#444',
					textAlign: 'left'
				}}>
					<form onSubmit={handleTempSlotSubmit}>
						<div>Slot: <input type="number" name="slot" min={0} max={tempSlots.length - 1} step={1} /></div>
						<div>Temperature: <input type="number" step={0.01} name="temperature" /> °C</div>
						<input type="submit" value="Set" />
					</form>
				</div>

				<b>Set time slot:</b>
				<div style={{
					border: '1px solid black',
					padding: '5px',
					background: '#444',
					textAlign: 'left'
				}}>
					<form onSubmit={handleTimeSlotSubmit}>
						<div>Slot: <input type="number" name="slot" min={0} max={timeSlots.length - 1} step={1} /></div>
						<div>Temperature slot: <input type="number" name="tempSlot" min={0} max={tempSlots.length - 1} step={1} /></div>
						<div>From: <input type="time" name="from" /></div>
						<div>To: <input type="time" name="to" /></div>
						<div><input type="checkbox" name="active" /><label>Enabled</label></div>
						<input type="submit" value="Set" />
					</form>
				</div>
				<b>{test}</b>
				<button onClick={() => setTest(t => t + 1)}>++</button>
			</div>
		</div>
	);
}
