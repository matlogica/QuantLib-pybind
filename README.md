# QuantLib-pybind
At the moment API coverage is very limited, but the idea is to replicate QuantLib SWIG. Pull requests are welcomed.

## To build:
```
git clone https://github.com/pybind/pybind11.git
git clone https://github.com/lballabio/quantlib.git
git clone https://github.com/matlogica/QuantLib-pybind.git

cd quantlib/
mkdir build
cd build/
cmake -DQL_USE_STD_SHARED_PTR=1 ..
make -j 8
cd ../../QuantLib-pybind
python3 setup.py install --user

python3 examples/cashflows.py
```
