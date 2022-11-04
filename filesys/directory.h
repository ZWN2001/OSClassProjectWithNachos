// directory.h 
//	Data structures to manage a UNIX-like directory of file names.
//
//      A directory is a table of pairs: <file name, sector #>,
//	giving the name of each file in the directory, and
//	where to find its file header (the data structure describing
//	where to find the file's data blocks) on disk.
//        We assume mutual exclusion is provided by the caller.
//用于管理类似于 UNIX 文件目录（文件名的形式）的数据结构。
//目录是一个表，表项为键值对: < file name, section # > ，
// 给出目录中所有文件名，以及对应的在磁盘上它的文件头的位置(描述在哪里找到文件的数据块的数据结构)。
//我们假设互斥锁是调用者提供的。

#include "copyright.h"

#ifndef DIRECTORY_H // if not define
#define DIRECTORY_H

#include "openfile.h"

#define FileNameMaxLen 9	//断言文件名长度不超过9位

// The following class defines a "directory entry", representing a file
// in the directory.  Each entry gives the name of the file, and where
// the file's header is to be found on disk.
//
// Internal data structures kept public so that Directory operations can
// access them directly.

///下面的类定义了一个“目录条目”，表示目录中的一个文件。每个条目提供文件的名称，以及文件头在磁盘上的位置。
///内部数据结构保持公开，以便 Directory 操作可以直接访问它们。

class DirectoryEntry {
  public:
    bool inUse;				//是否被占用
    int sector;				// 文件的文件头在磁盘的位置
    char name[FileNameMaxLen + 1];	// Text name for file, with +1 for the trailing '\0'
};

// The following class defines a UNIX-like "directory".  Each entry in
// the directory describes a file, and where to find it on disk.
//
// The directory data structure can be stored in memory, or on disk.
// When it is on disk, it is stored as a regular Nachos file.
//
// The constructor initializes a directory structure in memory; the
// FetchFrom/WriteBack operations shuffle the directory information
// from/to disk.

///类 Directory 定义了一个Unix风格的文件目录，文件目录中的每个条目描述了一个文件及其起始位置
///
///文件目录的数据结构被存储在内存或者硬盘中。存储在硬盘中的时候是作为一个常规的Nachos文件。

class Directory {
  public:
    Directory(int size); 		// Initialize an empty directory
					// with space for "size" files
    ~Directory();			// De-allocate the directory

    ///从硬盘中初始化一个文件目录
    void FetchFrom(OpenFile *file);  	// Init directory contents from disk
    ///将对目录内容的修改写入磁盘
    void WriteBack(OpenFile *file);	// Write modifications to 
					// directory contents back to disk
    ///查找名为name的文件的文件标头的扇区号
    int Find(char *name);		// Find the sector number of the 
					// FileHeader for file: "name"
    ///将名为name的文件加入文件目录
    bool Add(char *name, int newSector);  // Add a file name into the directory

    ///将名为name的文件从目录中删除
    bool Remove(char *name);		// Remove a file from the directory

    void List();			// Print the names of all the files
					//  in the directory
    void Print();			// Verbose print of the contents
					//  of the directory -- all the file
					//  names and their contents.

  private:
    int tableSize;			// Number of directory entries
    DirectoryEntry *table;		// Table of pairs: 
					// <file name, file header location> 

    int FindIndex(char *name);		// Find the index into the directory 
					//  table corresponding to "name"
};

#endif // DIRECTORY_H
