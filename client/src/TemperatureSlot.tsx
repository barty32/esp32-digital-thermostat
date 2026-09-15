


export interface TemperatureSlotProps {
	slotIndex: number;
	temperature: number;
}

export default function TemperatureSlot({slotIndex, temperature}: TemperatureSlotProps) {




	return (
		<div key={slotIndex} className='box'>
			<div>Slot: {slotIndex}</div>
			<div>Temperature: {temperature === null ? 'not set' : temperature + '°C'}</div>
		</div>
	)
}