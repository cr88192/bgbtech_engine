package bgb.vm;

public class Z_FileSystem extends Object
{

public native Z_IFile open(String name, String mode);
public native int mount(String dst, String fstype, String[] ops);
public native int umount(String mnt);

public native Z_IDir opendir(String name);
public native void closedir(Z_IDir dir);
public native String readdir(Z_IDir dir);
public native void rewinddir(Z_IDir dir);

public native int statflags(String name);
//public native int stat(String name, VFILE_Stat *st);
//public native int setstat(String name, VFILE_Stat *st, int fl);

public native int mkdir(String name);
public native int unlink(String name);
public native int link(String oldname, String newname);
public native int fscmd(int cmd, Object arg);

}