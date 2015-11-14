package bgb.vm;

public interface Z_IFileSystem
{

public Z_IFile open(String name, String mode);
public int mount(String dst, String fstype, String[] ops);
public int umount(String mnt);

public Z_IDir opendir(String name);
public void closedir(Z_IDir dir);
public String readdir(Z_IDir dir);
public void rewinddir(Z_IDir dir);

public int statflags(String name);
//public int stat(String name, VFILE_Stat *st);
//public int setstat(String name, VFILE_Stat *st, int fl);

public int mkdir(String name);
public int unlink(String name);
public int link(String oldname, String newname);
public int fscmd(int cmd, Object arg);

}