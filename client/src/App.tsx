import {FormEvent, useEffect, useState} from 'react'
// import reactLogo from './assets/react.svg'
// import viteLogo from '/vite.svg'
import './App.css'
import {  ITimeSlot} from './types';
import TemperatureGraph from './TemperatureGraph';
import TemperatureSlot from './TemperatureSlot';

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

	const [history, setHistory] = useState<number[][]>([[0, 0]]);

	const [temp, setTemp] = useState<number>(20.5);

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

	const fetchHistory = () => {
		fetch(`${ipAddress}/api/temperature/history`, {
			method: 'GET',
			mode: 'cors',
		})
			.then(response => response.json())
			.then((data: number[][]) => {
				data.sort((a, b) => a[0] - b[0]);
				//data = data.filter((point, index) => point.timestamp > 1736115867000);
				setHistory(data);
			})
			.catch(console.error);
		
		fetch(`${ipAddress}/api/temperature`, {
			method: 'GET',
			mode: 'cors',
		})
			.then(response => response.text())
			.then((data: string) => {
				setTemp(parseFloat(data));
			})
			.catch(console.error);
	}

	useEffect(() => {
		fetchTimeSlots();
	}, []);

	useEffect(() => {
		fetchTempSlots();
	}, []);

	useEffect(() => {
		fetchHistory();
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
		<div style={{display: 'flex', flexDirection: 'column', margin: '10px'}}>
			<h1>ESP32 Digital Thermostat</h1>
			<TemperatureGraph points={history} />
			
				
			<div style={{display: 'flex', flexDirection: 'column', margin: '10px', gap: '10px'}}>
				<div style={{display: 'flex', flexDirection: 'row', gap: '10px'}}>
					<div style={{flexGrow: 1}}>
						<b>Actual temperature:</b>
						<div className='box'>
							<div style={{fontSize: 35, fontWeight: 'bold', textAlign: 'center'}}>{temp.toString()} °C</div>
						</div>
					</div>
					<div style={{flexGrow: 1}}> 
						<b>Status:</b>
						<div className='box'>
							<div style={{fontSize: 35, fontWeight: 'bold', color: 'red', textAlign: 'center'}}>OFF</div>
						</div>
					</div>
				</div>
				<div style={{display: 'flex', justifyContent: 'space-between'}}>
					<div style={{
						display: 'flex',
						flexDirection: 'column',
						margin: '10px',
						gap: '10px',
						padding: '10px',
						borderRadius: '10px',
						background: '#222',
						border: '2px solid #111'
					}}>
						<b>Temperature slots:</b>
						{tempSlots.map((slot, index) => (
							<TemperatureSlot slotIndex={index} temperature={slot}/>
						))}
					</div>
					<div style={{display: 'flex', flexDirection: 'column', margin: '10px', gap: '10px'}}>
						<b>Time slots:</b>
						{timeSlots.map((timeSlot, index) => (
							<div key={index} className='box'>
								<div>Slot: {index}</div>
								<div>Start: {formatTime(new Date(timeSlot.startTime))}</div>
								<div>End: {formatTime(new Date(timeSlot.endTime))}</div>
								<div>Days: {timeSlot.days}</div>
								<div>Temperature slot: {timeSlot.temperatureSlot === null ? 'null' : `${timeSlot.temperatureSlot} (${tempSlots[timeSlot.temperatureSlot]} °C)`}</div>
								<div>Enabled: {timeSlot.active ? 'true' : 'false'}</div>
							</div>
						))}
					</div>

					{/* <b>Set temperature slot:</b>
					<div className='box'>
						<form onSubmit={handleTempSlotSubmit}>
							<div>Slot: <input type="number" name="slot" min={0} max={tempSlots.length - 1} step={1} style={{float: 'right'}}/></div>
							<div>Temperature: <input type="number" step={0.01} name="temperature" /> °C</div>
							<input type="submit" value="Set" />
						</form>
					</div>

					<b>Set time slot:</b>
					<div className='box'>
						<form onSubmit={handleTimeSlotSubmit}>
							<div>Slot: <input type="number" name="slot" min={0} max={timeSlots.length - 1} step={1} style={{float: 'right'}} /></div>
							<div>Temperature slot: <input type="number" name="tempSlot" min={0} max={tempSlots.length - 1} step={1} style={{float: 'right'}} /></div>
							<div>From: <input type="time" name="from" style={{float: 'right'}} /></div>
							<div>To: <input type="time" name="to" style={{float: 'right'}} /></div>
							<div><input type="checkbox" name="active" /><label>Enabled</label></div>
							<input type="submit" value="Set" />
						</form>
					</div> */}
				</div>
			</div>
		</div>
	);
}
