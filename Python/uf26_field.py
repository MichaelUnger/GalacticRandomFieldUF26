import numpy as np
from uf26 import UF26

gmf = UF26("expDisk")

# rms strength of the random field at one position ...
b = gmf.get_field(-8.178, 0, 0)
print('b_rms at the Sun is', b, 'microgauss')

# ... or at many
points = np.array([
    [0, 0, 0],
    [1, 3, 2],
    [-8.178, 0, 0]
])
for model in ['expDisk', 'ringDisk']:
    b = UF26(model).get_field_many(points)
    print(model, ': b_rms at\n', points, '\nis\n', b)
