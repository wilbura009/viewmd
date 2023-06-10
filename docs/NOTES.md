# Build Flatpack

flatpak-builder flatpak-build-dir <app-id>.json --force-clean --user --install
flatpak run <app-id>//master

----------------------------------------------------------------------

flatpak-builder flatpak-build-dir org.gnome.myapp.json --force-clean --user --install
flatpak run org.gnome.myapp
