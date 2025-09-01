## Linux常见配置问题

### 配置免密登陆

在~/.ssh目录下生成公钥和私钥

```sh
ssh-keygen -t rsa
```

此时./ssh目录下会生成两个文件`id_rsa`和`id_rsa.pub`

```sh
cat id_rsa.pub
```

上传公钥到服务器

```sh
ssh-copy-id username@hostname #ssh-copy-id ptx@192.168.10.123
```

此时服务器.ssh文件下会生成`authorized_keys`里面存放刚才复制的公钥，重启ssh服务

```sh
sudo service sshd restart
或
sudo systemctl restart sshd
```

远程登录

```sh
ssh username@hostname
```

退出登录

```sh
exit
```

**注意：**如果验证发现无法免密登陆排查以下原因。

1. 检查`.ssh`文件和`authorized_keys`的权限，`chmod 700 ~/.ssh && chmod 600 ~/.ssh/authorized_keys`，并且要归属当前登录用户。
   ```bash
   ~/.ssh → 700 (drwx------)
   ~/.ssh/authorized_keys → 600 (-rw-------)
   ~/.ssh/id_rsa → 600 (-rw-------)
   ~/.ssh/id_rsa.pub → 644 (-rw-r--r--)
   ~/.ssh/known_hosts → 644 (-rw-r--r--)
   ```

2. 打开 `/etc/ssh/sshd_config`，确认：
   ```bash
   PubkeyAuthentication yes
   AuthorizedKeysFile .ssh/authorized_keys
   ```

   





