#include <iostream>
#include <fstream>
#include <streambuf> // For std::streambuf

void run_high_volume_output() {
    // 1. Create the file stream object. This creates/opens the file.
    // The file will be placed in the current working directory of the executable.
    std::ofstream fileOut("blackjack_log.txt"); 

    if (!fileOut.is_open()) {
        std::cerr << "Error: Could not open log file!" << std::endl;
        return;
    }

    // 2. Save the original streambuf of std::cout (points to the console)
    std::streambuf* const original_cout_buf = std::cout.rdbuf(); 

    // 3. Redirect std::cout's buffer to the fileOut's buffer
    // Now, all uses of std::cout will write to the file.
    std::cout.rdbuf(fileOut.rdbuf()); 

    // --- Critical High-Volume Section ---
    for (int i = 0; i < 100000; ++i) {
        // This *looks* like console output, but it's going to the file!
        std::cout << "Game " << i << ": Simulation Result..." << "\n";
    }
    // The file is automatically flushed upon fileOut's destruction/close, 
    // but an explicit flush here is good practice before restoring the buffer.
    std::cout.flush(); 
    // ------------------------------------

    // 4. Restore the original streambuf (must be done before fileOut is destroyed!)
    std::cout.rdbuf(original_cout_buf); 

    std::cout << "Simulation finished. Results saved to blackjack_log.txt" << std::endl;
}