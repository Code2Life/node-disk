type DiskInfo = {
    volumes?: DiskInfo;
    free: number;
    total: number;
}

declare module 'node-disk' {
	
	export function getDisk(volume: string, callback: (error: Error, result: DiskInfo) => void): void;

	export function getDisk(callback: (error: Error, result: DiskInfo) => void): void;

	export function getDisk(volume?: string): Promise<DiskInfo>
	
}
