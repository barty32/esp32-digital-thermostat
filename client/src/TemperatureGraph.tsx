import {useMemo} from "react";
import {AxisOptions, Chart} from "react-charts";
//import {ITemperatureDataPoint} from "./types";


export default function TemperatureGraph({points}: {points: number[][]}) {

	type Series = {
		label: string,
		data: number[][]
	}

	// points = [
	// 	[Date.now() - 24 * 3600 * 1000, 20.1],
	// 	[Date.now() - 20 * 3600 * 1000, 19.8],
	// 	[Date.now() - 16 * 3600 * 1000, 20.5],
	// 	[Date.now() - 12 * 3600 * 1000, 21.5],
	// 	[Date.now() - 8 * 3600 * 1000, 21.7],
	// 	[Date.now() - 4 * 3600 * 1000, 21.2],
	// 	[Date.now(), 20.5]
	// ]
	
	const data: Series[] = [
		{
			label: 'Temperature',
			data: points
		},
	]
	
	const primaryAxis = useMemo((): AxisOptions<number[]> => ({
		getValue: datum => new Date(datum[0]),
		//hardMin: new Date(Date.now() - 120000 + 3600000),//24*60*60*1000
		//hardMax: new Date(Date.now() + 3600000),
		
		//min: new Date("2025-01-05T22:00:00"),
		elementType: 'area',
		shouldNice: false
		

		//showDatumElements: true
	}), []);

	const secondaryAxes = useMemo((): AxisOptions<number[]>[] => [{
		getValue: datum => datum[1],
	},], []);

	return (
		<div style={{
			display: 'flex',
			flexDirection: 'column',
			margin: '10px',
			padding: '20px',
			borderRadius: '10px',
			// width: '100%',
			height: '300px',
			background: '#333'
		}}>
			Last 24 hours
			<Chart
				options={{
					data,
					primaryAxis,
					secondaryAxes,
					dark: true,
					padding: {
						left: 10,
						right: 40,
						top: 20,
						bottom: 40
					}
				}}
			/>
		</div>
	);
}