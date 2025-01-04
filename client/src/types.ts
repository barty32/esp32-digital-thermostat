


export interface ITimeSlot {
	slot?: number;
	active: boolean;
	startTime: number;
	endTime: number;
	days: number;
	temperatureSlot: number;
}

export interface ITemperatureSlot {
	slot?: number;
	temperature: number;
}

