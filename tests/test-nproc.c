/*
nproc honors cgroup v2 CPU quotas
and was tested in coreutils on a Fedora 42 system as follows:

# Note we honor a limit anywhere in /proc/self/cgroup hierarchy
# so apply settings below in the parent cgroup of the current process
$ nested_cgroup=/sys/fs/cgroup/$(dirname $(cut -d/ -f2- /proc/self/cgroup))
$ echo $nested_cgroup
/sys/fs/cgroup/user.slice/user-1001.slice/user@1001.service/app.slice

# This test system has 4 CPUs
$ src/nproc
4

# Behave like MAX (1, (int)round(quota/period))
$ echo "100000 100000" > $nested_cgroup/cpu.max
$ src/nproc
1
$ echo "90000 100000" > $nested_cgroup/cpu.max
$ src/nproc
1
$ echo "140000 100000" > $nested_cgroup/cpu.max
$ src/nproc
1
$ echo "150000 100000" > $nested_cgroup/cpu.max
$ src/nproc
2

# Ensure NPROC_ALL takes precedence
$ echo "100000 100000" > $nested_cgroup/cpu.max
$ src/nproc --all
4

# Ensure OMP env vars have appropriate precedence
$ echo "200000 100000" > $nested_cgroup/cpu.max
$ OMP_NUM_THREADS=10 src/nproc
10
$ OMP_THREAD_LIMIT=10 src/nproc
2

# Ensure quota only reduces
$ echo "500000 100000" > $nested_cgroup/cpu.max
$ src/nproc
4

# Restore system to unlimited
$ echo "max 100000" > $nested_cgroup/cpu.max

# Test quota in root hierarchy
$ podman run --cpus=2 -i --rm fedora:latest /tmp/nproc
2
$ podman run --cpus=1.5 -i --rm fedora:latest /tmp/nproc
2
$ podman run --cpus=1.4 -i --rm fedora:latest /tmp/nproc
1
$ podman run --cpus=100 -i --rm fedora:latest /tmp/nproc
4

# Docker is similar to podman, but explicitly limits max allowable
$ docker run --cpus=1.4 -i --rm fedora:latest /tmp/nproc
1
$ docker run --cpus=100 -i --rm fedora:latest /tmp/nproc
docker: Error response from daemon:
range of CPUs is from 0.01 to 4.00, as there are only 4 CPUs
*/
