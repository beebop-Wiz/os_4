#ifndef EXT2FS_H
#define EXT2FS_H

#define E2FS_MAGIC 0xef53

typedef unsigned int uint;
typedef unsigned short ushort;

struct e2fs_superblock {
  uint s_inodes_count;
  uint s_blocks_count;
  uint s_r_blocks_count;
  uint s_free_blocks_count;
  uint s_free_inodes_count;
  uint s_first_data_block;
  uint s_log_block_size;
  uint s_log_frag_size;
  uint s_blocks_per_group;
  uint s_frags_per_group;
  uint s_inodes_per_group;
  uint s_mtime;
  uint s_wtime;
  ushort s_mnt_count;
  ushort s_max_mnt_count;
  ushort s_magic;
  ushort s_state;
  ushort s_errors;
  ushort s_minor_rev_level;
  uint s_lastcheck;
  uint s_checkinterval;
  uint s_creator_os;
  uint s_rev_level;
  ushort s_def_resuid;
  ushort s_def_resgid;
} __attribute__ ((packed));

struct e2fs_bdesc {
  uint bg_block_bitmap;
  uint bg_inode_bitmap;
  uint bg_inode_table;
  ushort bg_free_blocks_count;
  ushort bg_free_inodes_count;
  ushort bg_used_dirs_count;
  ushort bg_pad;
  unsigned char res[12];
} __attribute__ ((packed));

struct e2fs_inode {
  ushort i_mode;
  ushort i_uid;
  uint i_size;
  uint i_atime;
  uint i_ctime;
  uint i_mtime;
  uint i_dtime;
  ushort i_gid;
  ushort i_links_count;
  uint i_sectors;
  uint i_flags;
  uint i_osd1;
  uint i_bp0[12];
  uint i_bp1;
  uint i_bp2;
  uint i_bp3;
  uint i_generation;
  uint i_file_acl;
  uint i_dir_acl;
  uint i_faddr;
  uint i_osd2[3];
} __attribute__ ((packed));

struct e2fs_dirent {
  uint d_inode;
  ushort d_rec_len;
  unsigned char d_name_len;
  unsigned char d_file_type;
  unsigned char d_name[256];
};

#endif
