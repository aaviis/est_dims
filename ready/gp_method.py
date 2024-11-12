### Расчет корреляционной размерности по методу Грассбергера-Прокаччиа

import pandas as pd
import numpy as np
import plotly
import plotly.graph_objs as go
import decimal

### Берем некоторый готовый расчет зависимости корреляционного интеграла от масштаба. В данном примере мы взяли канторово множество на 500 000 точек с окном в 1, 5, 25.

file = "data\\corrint_kantor_500000_P25.txt"
#x = log(epsilon), y = log(CorrI) 

df = pd.read_csv(file, sep=';', header=None)
mm = df[0].unique().tolist()
dic_x = {}
dic_y = {}

for m in mm:
    dic_x[m] = np.round(df[df[0] == m][1].values, decimals = 6)
    dic_y[m] = np.round(df[df[0] == m][2].values, decimals = 6)

fig = go.Figure()

for m in mm:
    fig.add_trace(go.Scatter(
            x=dic_x[m], 
            y=dic_y[m],
            mode='lines+markers',
            line=dict(dash='dash'),
            name = f'm = {m}'
        ))

fig.update_layout(
    margin=dict(l=0, r=0, t=0, b=0),
    xaxis_title="Логарифм эпсилон",
    yaxis_title="Логарифм корреляционного интеграла CorrI",
    
)
fig.show()   


def mnk(x, y):  
    x = np.array(x)
    y = np.array(y)
    # Создание маски для некорректных значений
    mask = ~np.isnan(x) & ~np.isinf(x) & ~np.isnan(y) & ~np.isinf(y)

    # Избавляемся от некорректных данных
    x = x[mask]
    y = y[mask]

    # Проверка, остались ли данные после удаления некорректных значений
    if len(x) == 0 or len(y) == 0:
        raise ValueError("Все данные являются некорректными (NaN или бесконечности)")

    # Расчет средних значений x и y
    x_mean = np.mean(x)
    y_mean = np.mean(y)

    # Вычисление коэффициентов наклона и сдвига
    a = np.sum((x - x_mean) * (y - y_mean)) / np.sum((x - x_mean) ** 2)
    b = y_mean - a * x_mean # Наш наклон D() - корреляционный интеграл

    return a, b

# Проанализировав графики зависимости корреляционного интеграла от масштаба, находим линейный участок, где начинается насыщение, отмечаем эти точки на графике и от них уже считаем угол наклона D(m) стандартным методом mnk

# # Для P=1
# dot_x = {1:-6.87,
#          2:-6.2,
#          3:None,
#          4:None,
#          5:None}
# dot_y = {1:-17.29,
#          2:-21.37,
#          3:None,
#          4:None,
#          5:None}


# # Для P=5 
# dot_x = {1:-9.1,
#          2:-9.55,
#          3:None,
        #  4:None,
        #  5:None}
# dot_y = {1:-18.7,
#          2:-25.55,
#          3:None,
        # 4:None,
        #  5:None}

# Для P=25
dot_x = {1:-6.88,
         2:-6.5,
         3:None,
         4:None,
         5:None}
dot_y = {1:-17.28,
         2:-21.45,
         3:None,
         4:None,
         5:None}

df = pd.read_csv(file, sep=';', header=None)
mm = df[0].unique().tolist()
dic_x = {}
dic_y = {}
dic_approx = {}
name_approx = {}

for m in mm:
    dic_x[m] = np.round(df[df[0] == m][1].values, decimals = 6)
    dic_y[m] = np.round(df[df[0] == m][2].values, decimals = 6)
    
    if (dot_x[m] is not None):   
        i = 0
        array_x = []
        array_y = []
        for x in dic_x[m]:                 
            if (x <= dot_x[m]):
                array_x.append(dic_x[m][i])
                array_y.append(dic_y[m][i])
                i+=1   
        b, a = mnk(array_x,array_y)
        dic_approx[m] = [x*b+a for x in dic_x[m]]
        name_approx[m] = f'm:{m}, D:{round(b,2)}'


fig = go.Figure()

for m in mm:
    fig.add_trace(go.Scatter(
            x=dic_x[m], 
            y=dic_y[m],
            mode='lines+markers',
            line=dict(dash='dash'),
            name = f'm = {m}'
        ))

    
    if (dot_x[m] is not None):  
        fig.add_trace(go.Scatter(
            x=dic_x[m], 
            y=dic_approx[m],
            mode='lines',
            name = name_approx[m]
        ))
        
        fig.add_trace(go.Scatter(
                x=[dot_x[m]], 
                y=[dot_y[m]],
                mode='lines+markers',
                line=dict(dash='dash'),
                marker=dict(size=10),  # Размер маркера
                name = f'log_eps:{dot_x[m]}'
            ))

# Обновление макета графика
fig.update_layout(
    margin=dict(l=0, r=0, t=0, b=0),
    xaxis_title="Логарифм эпсилон",
    yaxis_title="Логарифм корреляционного интеграла CorrI",
    
)
fig.show()


# имеем:

# Для P=1:
# D(1) = 0.72
# D(2) = 1.07

# Для P=5:
# D(1) = 0.97
# D(2) = 0

# Для P=25:
# D(1) = 0.72
# D(2) = 1.35


# Наличие насыщения говорит о том, что сигнал генерируется динамической системой. Мы получили примерные оценки корреляционной размерности аттрактора динамической системы, породившей исследуемый сигнал.
# Можно утверждать, что размерность фазового пространства этой динамической системы не превышает `2D+1` 

# А дальше построить график D(m). Насыщаемость этого графика даст независимую оценку размерности вложения

fig = go.Figure()

fig.add_trace(go.Scatter(
            x=[1, 2], 
            y=[0.97, 0],
            mode='lines',
            line=dict(dash='dash'),
            name = f'p=5'
        ))    
fig.add_trace(go.Scatter(
            x=[1, 2], 
            y=[0.72, 1.07],
            mode='lines',
            line=dict(dash='dash'),
            name = f'p=1'
        ))   
fig.add_trace(go.Scatter(
            x=[1, 2], 
            y=[0.75, 1.35],
            mode='lines',
            line=dict(dash='dash'),
            name = f'p=25'
        ))   
    
# Обновление макета графика
fig.update_layout(
    margin=dict(l=0, r=0, t=0, b=0),
    xaxis_title="Размерность пространства",
    yaxis_title="Фрактальная размерность",
    
)
fig.show()


# Канторово множество имеет размерность 0.63, поэтому график скучный. По идее тут должен быть рост фрактальной размерности до некоторого уровня, а потом насыщение при некоторой размерности m, по которой мы и будем оценивать нашу размерность системы: 2*D+1

# Далее, следует повторить исследование для разного p (4,15) и сравнить полученную оценку размерности.

# Реконструируем портрет аттрактора данного множества при размерности m = 2. Для этого построим из элементов множества многомерные вектора типа: {(x(t),x(t+τ)),(x(t+1),x(t+1+τ)),…}, где p = 5 фиксированное окно


import numpy as np
import matplotlib.pyplot as plt

file = "data\\set_kantor_500000.txt"

df = pd.read_csv(file, sep=';', header=None)
x = df[0].unique().tolist()
m = 2
p = 5

x_data = []
y_data = []

for i in range(len(x) - m * p):
    if (i>10000): break
    x_data.append(x[i])
    y_data.append(x[i + p])




# Преобразование векторов в массивы
x_data = np.array(x_data)
y_data = np.array(y_data)

# Построение графика
plt.figure(figsize=(10, 6))
plt.plot(x_data, y_data, lw=1)
plt.title('Портрет аттрактора')
plt.xlabel('x(t)')
plt.ylabel('x(t + τ)')
plt.grid()
plt.show()

import matplotlib as mpl
mpl.rcParams['agg.path.chunksize'] = 500000
mpl.rcParams['path.simplify_threshold'] = 0.2