import os

# 获取当前用户的主目录
home_dir = os.path.expanduser("~")
print(home_dir)

# 将路径中的用户主目录缩写扩展为完整路径
full_path = os.path.expanduser("~/Documents")
print(full_path)
