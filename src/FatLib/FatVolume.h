/**
 * Copyright (c) 2011-2020 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef FatVolume_h
#define FatVolume_h
#include "FatPartition.h"
#include "FatFile.h"
/**
 * \file
 * \brief FatVolume class
 */
//------------------------------------------------------------------------------
/**
 * \class FatVolume
 * \brief Integration class for the FatLib library.
 */
class FatVolume : public  FatPartition {
 public:
  /**
   * Initialize an FatVolume object.
   * \param[in] dev Device block driver.
   * \param[in] setCwv Set current working volume if true.
   * \param[in] part partition to initialize.
   * \return true for success or false for failure.
   */
  bool begin(BlockDevice* dev, bool setCwv = true, uint8_t part = 1) {
    if (!init(dev, part)) {
      return false;
    }
    if (!chdir()) {
      return false;
    }
    if (setCwv || !m_cwv) {
      m_cwv = this;
    }
    return true;
  }
  /** Change global current working volume to this volume. */
  void chvol() {m_cwv = this;}

  /**
   * Set volume working directory to root.
   * \return true for success or false for failure.
   */
  bool chdir() {
    m_vwd.close();
    return m_vwd.openRoot(this);
  }
  /**
   * Set volume working directory.
   * \param[in] path Path for volume working directory.
   * \return true for success or false for failure.
   */
  bool chdir(const char *path);

  //----------------------------------------------------------------------------
  /**
   * Test for the existence of a file.
   *
   * \param[in] path Path of the file to be tested for.
   *
   * \return true if the file exists else false.
   */
  bool exists(const char* path) {
    FatFile tmp;
    return tmp.open(this, path, O_RDONLY);
  }
  //----------------------------------------------------------------------------
  /** List the directory contents of the volume root directory.
   *
   * \param[in] pr Print stream for list.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   *
   * \return true for success or false for failure.
   */
  bool ls(print_t* pr, uint8_t flags = 0) {
    return m_vwd.ls(pr, flags);
  }
  //----------------------------------------------------------------------------
  /** List the contents of a directory.
   *
   * \param[in] pr Print stream for list.
   *
   * \param[in] path directory to list.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   *
   * \return true for success or false for failure.
   */
  bool ls(print_t* pr, const char* path, uint8_t flags) {
    FatFile dir;
    return dir.open(this, path, O_RDONLY) && dir.ls(pr, flags);
  }
  //----------------------------------------------------------------------------
  /** Make a subdirectory in the volume root directory.
   *
   * \param[in] path A path with a valid name for the subdirectory.
   *
   * \param[in] pFlag Create missing parent directories if true.
   *
   * \return true for success or false for failure.
   */
  bool mkdir(const char* path, bool pFlag = true) {
    FatFile sub;
    return sub.mkdir(vwd(), path, pFlag);
  }
  //----------------------------------------------------------------------------
  /** open a file
   *
   * \param[in] path location of file to be opened.
   * \param[in] oflag open flags.
   * \return a File32 object.
   */
  File32 open(const char *path, oflag_t oflag = O_RDONLY) {
    File32 tmpFile;
    tmpFile.open(this, path, oflag);
    return tmpFile;
  }
  //----------------------------------------------------------------------------
  /** Remove a file from the volume root directory.
   *
   * \param[in] path A path with a valid name for the file.
   *
   * \return true for success or false for failure.
   */
  bool remove(const char* path) {
    FatFile tmp;
    return tmp.open(this, path, O_WRONLY) && tmp.remove();
  }
  //----------------------------------------------------------------------------
  /** Rename a file or subdirectory.
   *
   * \param[in] oldPath Path name to the file or subdirectory to be renamed.
   *
   * \param[in] newPath New path name of the file or subdirectory.
   *
   * The \a newPath object must not exist before the rename call.
   *
   * The file to be renamed must not be open.  The directory entry may be
   * moved and file system corruption could occur if the file is accessed by
   * a file object that was opened before the rename() call.
   *
   * \return true for success or false for failure.
   */
  bool rename(const char *oldPath, const char *newPath) {
    FatFile file;
    return file.open(vwd(), oldPath, O_RDONLY) && file.rename(vwd(), newPath);
  }
  //----------------------------------------------------------------------------
  /** Remove a subdirectory from the volume's working directory.
   *
   * \param[in] path A path with a valid name for the subdirectory.
   *
   * The subdirectory file will be removed only if it is empty.
   *
   * \return true for success or false for failure.
   */
  bool rmdir(const char* path) {
    FatFile sub;
    return sub.open(this, path, O_RDONLY) && sub.rmdir();
  }
  //----------------------------------------------------------------------------
  /** Truncate a file to a specified length.  The current file position
   * will be at the new EOF.
   *
   * \param[in] path A path with a valid name for the file.
   * \param[in] length The desired length for the file.
   *
   * \return true for success or false for failure.
   */
  bool truncate(const char* path, uint32_t length) {
    FatFile file;
    return file.open(this, path, O_WRONLY) && file.truncate(length);
  }
  //----------------------------------------------------------------------------
  /** Set the working directory's current position to zero. */
  void vwdRewind() { m_vwd.seekSet(0); }
  //----------------------------------------------------------------------------
  /** \return The current position for a file or directory. */
  uint32_t vwdCurPosition() const { return m_vwd.curPosition(); }
  //----------------------------------------------------------------------------
  /** Sets the working directory's current position.
   *
   * \param[in] pos The new position of sub entry.
   *
   * \return true for success or false for failure.
   */
  bool vwdSeekSet(uint32_t pos) { return m_vwd.seekSet(pos); }
  //----------------------------------------------------------------------------
  /** Open the next file or subdirectory in a directory.
   *
   * \param[in] oflag bitwise-inclusive OR of open flags.
   *                  See see FatFile::open(FatFile*, const char*, uint8_t).
   *
   * \return true for success or false for failure.
   */
  File32 vwdOpenNext(oflag_t oflag = O_RDONLY) {
    File32 tmpFile;
    tmpFile.openNext(vwd(), oflag);
    return tmpFile;
  }
  //----------------------------------------------------------------------------
  /** \return All error bits. */
  uint8_t vwdGetError() const { return m_vwd.m_error; }
  //----------------------------------------------------------------------------
  /** Remove current working directory.
   *
   * Current working directory will be removed only if it is empty.
   *
   * \return true for success or false for failure.
   */
  bool vwdRmdir() {
    return m_vwd.rmdir() && chdir();
  }
  //----------------------------------------------------------------------------
  /** Recursively delete current working directory and all contained files.
   *
   * This is like the Unix/Linux 'rm -rf *' if called with the root directory
   * hence the name.
   *
   * Warning - This will remove all contents of the directory including
   * subdirectories.  The directory will then be removed if it is not root.
   * The read-only attribute for files will be ignored.
   *
   * \note This function should not be used to delete the 8.3 version of
   * a directory that has a long name.  See remove() and rmdir().
   *
   * \return true for success or false for failure.
   */
  bool vwdRmRfStar() {
    return m_vwd.rmRfStar() && chdir();
  }
  //----------------------------------------------------------------------------
  /**
   * Test for the existence of a file.
   *
   * \param[in] relative_path Relative path of the file to be tested for.
   *
   * \return true if the file exists else false.
   */
  bool relExists(const char* relative_path) {
    FatFile tmp;
    return tmp.open(vwd(), relative_path, O_RDONLY);
  }
  //----------------------------------------------------------------------------
  /** Remove a file from the volume root directory.
   *
   * \param[in] relative_path Relative path of the file to be tested for.
   *
   * \return true for success or false for failure.
   */
  bool relRemove(const char* relative_path) {
    FatFile tmp;
    return tmp.open(vwd(), relative_path, O_WRONLY) && tmp.remove();
  }
  //----------------------------------------------------------------------------
  /** Remove a subdirectory from the current working directory.
   *
   * \param[in] path A path with a valid name for the subdirectory.
   *
   * The subdirectory file will be removed only if it is empty.
   *
   * \return true for success or false for failure.
   */
  bool relRmdir(const char* relative_path) {
    FatFile sub;
    return sub.open(vwd(), relative_path, O_RDONLY) && sub.rmdir();
  }
  //----------------------------------------------------------------------------
  /** \return True if this is a directory. */
  bool isDir(const char* relative_path) {
    FatFile dir;
    bool result = false;
    if (dir.open(vwd(), relative_path, O_RDONLY)) {
      if (dir.isDir()) {
        result = true;
      }
      dir.close();
    }
    return result;
  }
  //----------------------------------------------------------------------------
  /** \return True if this is a normal file. */
  bool isFile(const char* relative_path) {
    FatFile file;
    bool result = false;
    if (file.open(vwd(), relative_path, O_RDONLY)) {
      if (file.isFile()) {
        result = true;
      }
      file.close();
    }
    return result;
  }
#if ENABLE_ARDUINO_SERIAL
   /** List the directory contents of the root directory to Serial.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   *
   * \return true for success or false for failure.
   */
  bool ls(uint8_t flags = 0) {
    return ls(&Serial, flags);
  }
  /** List the directory contents of a directory to Serial.
   *
   * \param[in] path directory to list.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   *
   * \return true for success or false for failure.
   */
  bool ls(const char* path, uint8_t flags = 0) {
    return ls(&Serial, path, flags);
  }
#endif  // ENABLE_ARDUINO_SERIAL
#if ENABLE_ARDUINO_STRING
  //----------------------------------------------------------------------------
  /**
   * Set volume working directory.
   * \param[in] path Path for volume working directory.
   * \return true for success or false for failure.
   */
  bool chdir(const String& path) {
    return chdir(path.c_str());
  }
   /**
   * Test for the existence of a file.
   *
   * \param[in] path Path of the file to be tested for.
   *
   * \return true if the file exists else false.
   */
  bool exists(const String& path) {
    return exists(path.c_str());
  }
  /** Make a subdirectory in the volume root directory.
   *
   * \param[in] path A path with a valid name for the subdirectory.
   *
   * \param[in] pFlag Create missing parent directories if true.
   *
   * \return true for success or false for failure.
   */
  bool mkdir(const String& path, bool pFlag = true) {
    return mkdir(path.c_str(), pFlag);
  }
  /** open a file
   *
   * \param[in] path location of file to be opened.
   * \param[in] oflag open flags.
   * \return a File32 object.
   */
  File32 open(const String& path, oflag_t oflag = O_RDONLY) {
    return open(path.c_str(), oflag );
  }
  /** Remove a file from the volume root directory.
   *
   * \param[in] path A path with a valid name for the file.
   *
   * \return true for success or false for failure.
   */
  bool remove(const String& path) {
    return remove(path.c_str());
  }
  /** Rename a file or subdirectory.
   *
   * \param[in] oldPath Path name to the file or subdirectory to be renamed.
   *
   * \param[in] newPath New path name of the file or subdirectory.
   *
   * The \a newPath object must not exist before the rename call.
   *
   * The file to be renamed must not be open.  The directory entry may be
   * moved and file system corruption could occur if the file is accessed by
   * a file object that was opened before the rename() call.
   *
   * \return true for success or false for failure.
   */
  bool rename(const String& oldPath, const String& newPath) {
    return rename(oldPath.c_str(), newPath.c_str());
  }
  /** Remove a subdirectory from the volume's working directory.
   *
   * \param[in] path A path with a valid name for the subdirectory.
   *
   * The subdirectory file will be removed only if it is empty.
   *
   * \return true for success or false for failure.
   */
  bool rmdir(const String& path) {
    return rmdir(path.c_str());
  }
  /** Truncate a file to a specified length.  The current file position
   * will be at the new EOF.
   *
   * \param[in] path A path with a valid name for the file.
   * \param[in] length The desired length for the file.
   *
   * \return true for success or false for failure.
   */
  bool truncate(const String& path, uint32_t length) {
    return truncate(path.c_str(), length);
  }
#endif  // ENABLE_ARDUINO_STRING

 private:
  friend FatFile;
  static FatVolume* cwv() {return m_cwv;}
  FatFile* vwd() {return &m_vwd;}
  static FatVolume* m_cwv;
  FatFile m_vwd;
};
#endif  // FatVolume_h
