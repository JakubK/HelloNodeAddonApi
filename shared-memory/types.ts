export type SharedMemoryType = {
    /**
     * Initializes shared memory mapping
     * @returns 0 for success
     */
    initFileMapping: () => number;
    /**
     * 
     * @param payload any string
     * @returns 0 for success
     */
    sendCommand: (payload: string) => number;
    /**
     * 
     * @returns string present in the command buffer
     */
    readCommand: () => string;
    /**
     * 
     * @param payload any string
     * @returns 0 for success
     */
    writeResponse: (payload: string) => number;
    /**
     * 
     * @returns string present in response buffer
     */
    readResponse: () => string;
    /**
     * 
     * @returns Executes cleanup methods for the shared memory
     */
    cleanFileMapping: () => void;
};