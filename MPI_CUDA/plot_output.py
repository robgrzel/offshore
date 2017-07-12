import numpy as np
import matplotlib.pyplot as plt

path = "cmake-build-debug"

cases1 = ["buoyInEqulibrium", "buoyOutEquilibrium", "buoyInEquilibriumDamped", "buoyOutEquilibriumDamped"]
cases2 = ["buoyInEqulibriumWave", "buoyOutEquilibriumWave", "buoyInEquilibriumDampedWave",
          "buoyOutEquilibriumDampedWave"]


def floating_cylinder_plot(case, cnt):
	try:
		t = np.fromfile(path + "/" + case + "_FloatingCylinder_t.txt", float, -1, ",\n");
		z = np.fromfile(path + "/" + case + "_FloatingCylinder_z.txt", float, -1, ",\n");
		dotz = np.fromfile(path + "/" + case + "_FloatingCylinder_dotz.txt", float, -1, ",\n");
		ddotz = np.fromfile(path + "/" + case + "_FloatingCylinder_ddotz.txt", float, -1, ",\n");
		
		F = np.fromfile(path + "/" + case + "_FloatingCylinder_Fsum.txt", float, -1, ",\n");
		Faddm = np.fromfile(path + "/" + case + "_FloatingCylinder_Faddm.txt", float, -1, ",\n");
		Frest = np.fromfile(path + "/" + case + "_FloatingCylinder_Frest.txt", float, -1, ",\n");
		Fdamp = np.fromfile(path + "/" + case + "_FloatingCylinder_Fdamp.txt", float, -1, ",\n");
		Fdrag = np.fromfile(path + "/" + case + "_FloatingCylinder_Fdrag.txt", float, -1, ",\n");
		Flift = np.fromfile(path + "/" + case + "_FloatingCylinder_Flift.txt", float, -1, ",\n");
		
		n = t.size
		
		f = plt.figure()
		plt.title(case)
		plt.plot(t, z, label="z")
		plt.plot(t, dotz, label="dotz")
		plt.plot(t, ddotz, label="ddotz")
		plt.legend()
		
		f = plt.figure()
		plt.title(case)
		plt.plot(t, F, label="Fsum")
		plt.plot(t, Frest, label="Frest")
		plt.plot(t, Fdamp, label="Fdamp")
		plt.plot(t, Faddm, label="Faddm")
		plt.plot(t, Fdrag, label="Fdrag")
		plt.plot(t, Flift, label="Flift")
		plt.legend()
		plt.show()
	
	except:
		pass


def monopile_stuck_plot_zi(case, zi):
	try:
		print(path + "/" + case + "_StuckCylinder_t.txt");
		t = np.fromfile(path + "/" + case + "_StuckCylinder_t.txt", float, -1, ",\n");
		
		F = np.fromfile(path + "/" + case + "_StuckCylinder_Fsum.txt", float, -1, ",\n");
		Faddm = np.fromfile(path + "/" + case + "_StuckCylinder_Faddm.txt", float, -1, ",\n");
		Fdrag = np.fromfile(path + "/" + case + "_StuckCylinder_Fdrag.txt", float, -1, ",\n");
		
		n = t.size
		
		Fx, Fy, Fz = F[0::3], F[1::3], F[2::3]
		Faddmx = Faddm[0::3]
		Fdragx = Fdrag[0::3]
		print(Fx.shape, Fy.shape, Fz.shape)
		
		plt.title(case)
		plt.plot(t, Fx, label="Fsum(%d)" % zi)
		plt.plot(t, Faddmx, label="Faddm(%d)" % zi)
		plt.plot(t, Fdragx, label="Fdrag(%d)" % zi)
	
	
	except Exception as err:
		print(err)


def monopile_stuck_plot(case):
	try:
		print(path + "/" + case + "_t");
		R = np.fromfile(path + "/"+case+"_Results.txt", float, -1, "\n");
		
		n = R.size
		
		R.shape = n // 7, 7
		
		t, Fx, Fy, Fz, Mx, My, Mz = R.T
		
		m = int(50/0.005)
	
	
		plt.plot(t[:m], Fx[:m], 'k.-', label="Fx(t)")
		plt.plot(t[:m], Mx[:m], 'k.-', label="Mx(t)")
		plt.legend()
		plt.show()
	
	except Exception as err:
		print(err)


def floating_cylinder_plot1(case):
	try:
		print(path + "/" + case + "_t");
		R = np.fromfile(path + "/" + case + "_Results.txt", float, -1, "\n");
		
		n = R.size
		
		
		
		R.shape = n // 43, 43
		
		t, Theta, Eta,\
		Mx, My, Mz, \
		Fx,	Fy, Fz, \
		Fdrx, Fdry, Fdrz, \
		Finx, Finy, Finz, \
		Fadx, Fady, Fadz, \
		Fdax, Fday, Fdaz, \
		Frex, Frey, Frez, \
		Fwavez, \
		x, y, z, \
		dotx, doty, dotz, \
		ddotx, ddoty, ddotz, \
		u, v, w, \
		dotu, dotv, dotw, \
		ddotu, ddotv, ddotw = R.T
		
		f = plt.figure()
		
		m = int(300/0.005)
		
		plt.plot(t[:m], Fz[:m], label="Fz(t)")
		plt.plot(t[:m], Fwavez[:m], label="Fwavez(t)")
		plt.plot(t[:m], Finz[:m], label="Finz(t)")
		plt.plot(t[:m], Fadz[:m], label="Fadz(t)")
		plt.plot(t[:m], Fdaz[:m], label="Fdaz(t)")
		plt.plot(t[:m], Fdrz[:m], label="Mdrz(t)")
		plt.plot(t[:m], Frez[:m], label="Mrez(t)")
		plt.legend()
		
		f = plt.figure()
		plt.plot(t[:m], z[:m], label="z(t)")
		plt.plot(t[:m], dotz[:m], label="dotz(t)")
		plt.plot(t[:m], ddotz[:m], label="ddotz(t)")
		plt.legend()
	
		f = plt.figure()
		plt.plot(t[:m], w[:m], label="w(t)")
		plt.plot(t[:m], dotw[:m], label="dotw(t)")
		plt.plot(t[:m], ddotw[:m], label="ddotw(t)")
		plt.legend()
		plt.show()

	except Exception as err:
		print(err)

if 0:
	cnt = 0
	for case in cases:
		floating_cylinder_plot(case, cnt);

if 0:
	case = "FloatingCylinder_noEquilibrium_onWave_onDamp"
	floating_cylinder_plot1(case)

if 0:
	case = "FloatingCylinder_noEquilibrium_onWave_noDamp"
	floating_cylinder_plot1(case)


if 1:
	f = plt.figure()
	case = "MonopileStuckSeabed"
	monopile_stuck_plot(case)

if 0:
	f = plt.figure()
	case = "MonopileStuck_zi=%.6f"
	for zi in [-35., -25., -15., -5.]:
		monopile_stuck_plot(case % zi, zi)
	
	plt.legend()
	plt.show()
