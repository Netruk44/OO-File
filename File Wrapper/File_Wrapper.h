/* File Wrapper
 * Purpose: Supply an object-oriented way of accessing files.
 *
 * Version: 0.1
 * Last modified: 1/4/2012
 */

#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include "File_Exception.h"

namespace File
{
  // Bit flags used to specify what to do when opening a file.
  // By default: MODE_WRITE | MODE_BINARY | MODE_OVERWRITE
  enum Mode
  {
    // Privileges
    MODE_WRITE  =    0x00000001, // Read-write mode.
    MODE_READ   =    0x00000002, // Read only mode.

    // Translation mode
    MODE_BINARY =    0x00000004, // Binary mode. No translation of newlines
    MODE_TEXT   =    0x00000008, // Text mode. Translation of newlines occurs.

    // What to do if the file already exists
    MODE_CLEAR =     0x00000010, // Erase the file and create a blank one.
    MODE_APPEND =    0x00000020, // Start off writing to the end of the file. Can be moved to before the end.
    MODE_OVERWRITE = 0x00000040, // Start off writing at the beginning of the file.

    MODE_PROTECT =   0x00000080, // Protect contents of the file (if any exist), used in conjunction with APPEND and OVERWRITE

    MODE_CREATE =    0x00000100, // Create the file if it does not exist. If the file exists, does nothing special.



    // Cannot be used in constructor
    MODE_SAME =      0x80000000, // Open another file in the same way the previous file was opened.
  };

  // The file class to be used when dealing with files.
  class File
  {
  public:
    /* Opens the file with a given filename.
     *
     * filename: The name of the file to open. Can contain a path.
     * mode: The mode specifying how the file should be opened. See: Mode enum.
     *
     * Throws: See: Open
     */
    File(const char* filename, Mode mode = static_cast<Mode>(MODE_WRITE | MODE_BINARY | MODE_OVERWRITE)) throw(File_Exception);

    /* Copies the buffer and status of the rhs file. All edits made
     * to rhs since file was opened will be copied over to the newly
     * created file. However, changes made to rhs after copy will not
     * be automatically applied.
     *
     * rhs: The file to copy status and buffer from.
     *
     * Throws: E_OUTOFMEMORY - new had an error while copying the filename/buffer.
     */
    File(const File& rhs) throw(File_Exception);

    /* If a file is open, it will be closed first. Then does the
     * same thing as the copy constructor. Status of file is copied
     * as-is. All changes made afterwards are not copied over.
     *
     * rhs: The file to copy status and buffer from.
     *
     * Throws: E_OUTOFMEMORY - new had an eeror while copying the filename/buffer.
     * Status after Throw: File is closed.
     */
    File& operator=(const File& rhs);

    /* Automatically calls Close on the file.
     */
    ~File();

    /* Opens a file with a specified filename in the given mode. If
     * there's a file already open, it will be closed first.
     *
     * filename: The file to open.
     * mode: The mode specifying how to open the file. See: Mode enum.
     *
     * Throws: E_FOPENERROR   - fopen didn't return a valid file.
     *         E_FILETOOLARGE - The largest file that can be opened is INT_MAX. The file is larger than that.
     *         E_OUTOFMEMORY  - new had an error allocating the filename or buffer for the file.
     * Status after Throw: File is closed.
     */
    void Open(const char* filename, Mode mode = MODE_SAME) throw(File_Exception);

    /* Writes out changes to the file and closes the file.
     * File will be re-created if it has since been deleted.
     *
     * Throws: E_FOPENERROR - fopen didn't return a valid file.
     * Status after Throw: No change. File is not closed.
     */
    void Close() throw(File_Exception);

    /* Writes out the current status of the buffer to a file.
     *
     * filename: The file to write out to.
     *
     * Throws: E_FOPENERROR - fopen didn't return a valid file.
     * Status after Throw: No change.
     */
    void WriteFile(const char* filename) const throw(File_Exception);

    /* Whether or not the end of the file has been reached.
     * You do not need to have tried to read past the end of the
     * file for this to be true.
     *
     * Returns: True - The EOF has been reached. There is nothing left to read.
     *          False - EOF has not yet been reached.
     */
    bool EndOfFile() const;

    /* Gets the next character in the file. Optionally, can skip
     * whitespace while looking for the next character.
     *
     * ignoreWhitespace: Whether or not to get the next non-whitespace
     *                    character.
     *
     * Returns: The next character in the file.
     */
    char GetChar(bool ignoreWhitespace = false);

    /* Gets the current position of the internal buffer pointer
     *
     * Returns: Buffer pointer position.
     */
    unsigned GetPos() const;

    /* Gets the next string in the file. Reads from the next non-whitespace
     * character inside the buffer, until the next terminator character,
     * EOF is reached, or untl maxLength is reached. The string is
     * guranteed to be null-terminated.
     *
     * outputString: Where the string will be stored.
     * maxLength: The largest the string can be.
     * terminator: The signal of the end of the string.
     *
     * Returns: The number of characters read. Includes null terminator.
     */
    unsigned GetString(char* outputString, unsigned maxLength, char terminator = '\n');

    /* Puts a character onto the file buffer.
     *
     * character: What to put onto the buffer.
     * ignoreErrors: True - Do nothing when an error occurs. (e.g. writing protected memory)
     *               False - Throw when an error occurs.
     *
     * Throws: E_PROTECTED   - Attempting to write into protected area of the file.
     *         E_PROTECTED   - Attempting to write into a read-only file.
     *         E_OUTOFMEMORY - New failed while trying to resize internal buffer.
     * Status after Throw: No change.
     */
    void PutChar(char character, bool ignoreErrors = false) throw (File_Exception);

    /* Increases the internal buffer size to a desired amount.
     * If lower than the current buffer size, does nothing.
     *
     * desiredSize: How large the internal buffer should be.
     * Throws: E_OUTOFMEMORY - New failed while trying to resize internal buffer.
     * Status after Throw: No change.
     */
    void Resize(unsigned desiredSize) throw (File_Exception);

    /* Put a null-terminated string onto the file buffer.
     *
     * string: The string to put onto the buffer.
     * ignoreErrors: True - Do nothing when an error occurs. (e.g. writing protected memory)
     *               False - Throw when an error occurs.
     *
     * Throws: E_PROTECTED   - Attempting to write into protected area of the file.
     *         E_PROTECTED   - Attempting to write into a read-only file.
     *         E_OUTOFMEMORY - New failed while trying to resize internal buffer.
     * Status after Throw: No change.
     */
  private:
    File();

    // Copies over the data and the status of the other file.
    void CopyStatus(const File& other) throw(File_Exception);

    // Applies defaults to a given mode.
    void ApplyDefaults(Mode& mode);

    bool open_; // Whether or not the file is currently opened.

    char* filename_; // The file we have open
    char* file_;     // The internal buffer that contains the contents of the file.

    unsigned fileSize_;   // The size of the file inside the buffer.
    unsigned bufferSize_; // The size of the internal buffer.
    unsigned currentPos_; // The current position of where we're reading/writing at in the buffer.
    unsigned protectEnd_; // The last byte in the file that is protected from writing to.
    Mode     mode_;       // How the file is opened.
  };
}

#endif