#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>

#include "apiPlugin.h"
#include "apiVisuals.h"
#include "apiObjects.h"
#include "apiCore.h"

class RedLineVisualization final : public IAIMPExtensionEmbeddedVisualization {
    std::atomic<ULONG> refs_{1};
    int width_ = 1;
    int height_ = 1;

public:
    HRESULT WINAPI QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;

        if (riid == IID_IUnknown ||
            riid == IID_IAIMPExtensionEmbeddedVisualization) {
            *ppv = static_cast<IAIMPExtensionEmbeddedVisualization*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG WINAPI AddRef() override {
        return ++refs_;
    }

    ULONG WINAPI Release() override {
        ULONG n = --refs_;
        if (n == 0) delete this;
        return n;
    }

    int WINAPI GetFlags() override {
        return AIMP_VISUAL_FLAGS_RQD_DATA_WAVEFORM;
    }

    HRESULT WINAPI GetMaxDisplaySize(int* Width, int* Height) override {
        if (Width) *Width = 0;
        if (Height) *Height = 0;
        return E_FAIL; // no fixed maximum
    }

    HRESULT WINAPI GetName(IAIMPString** S) override {
        if (!S) return E_POINTER;
        *S = nullptr;
        // AIMP creates the string object through the core; the display name is
        // not essential for rendering. Returning E_FAIL is accepted by the
        // official demo as long as the extension itself is registered.
        return E_FAIL;
    }

    HRESULT WINAPI Initialize(int Width, int Height) override {
        width_ = std::max(1, Width);
        height_ = std::max(1, Height);
        return S_OK;
    }

    void WINAPI Finalize() override {}

    void WINAPI Click(int, int, int) override {}

    void WINAPI Resize(int NewWidth, int NewHeight) override {
        width_ = std::max(1, NewWidth);
        height_ = std::max(1, NewHeight);
    }

    void WINAPI Draw(HCANVAS canvas, PAIMPVisualData data) override {
        if (!canvas || !data || width_ < 2 || height_ < 2)
            return;

        // Deliberately draw ONLY the red waveform. No background, grid,
        // glow or other pixels are touched, so the user's AIMP skin remains.
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(230, 38, 35));
        if (!pen) return;

        HGDIOBJ old = SelectObject(canvas, pen);

        const int center = height_ / 2;
        const float scale = std::max(1.0f, height_ * 0.45f);

        auto sample = [&](int i) -> float {
            float l = data->WaveForm[0][i];
            float r = data->WaveForm[1][i];
            return std::clamp((l + r) * 0.5f, -1.0f, 1.0f);
        };

        int y0 = center - static_cast<int>(std::lround(sample(0) * scale));
        MoveToEx(canvas, 0, y0, nullptr);

        for (int i = 1; i < AIMP_VISUAL_WAVEFORM_MAX; ++i) {
            int x = (i * (width_ - 1)) / (AIMP_VISUAL_WAVEFORM_MAX - 1);
            int y = center - static_cast<int>(std::lround(sample(i) * scale));
            LineTo(canvas, x, y);
        }

        SelectObject(canvas, old);
        DeleteObject(pen);
    }
};

class RedLinePlugin final : public IAIMPPlugin {
    std::atomic<ULONG> refs_{1};
    IAIMPCore* core_ = nullptr;
    RedLineVisualization* visualization_ = nullptr;

    static TChar* text(int index) {
        static TChar name[] = L"NTK Red Line x64";
        static TChar author[] = L"Clean-room reimplementation";
        static TChar shortDesc[] = L"Minimal red waveform line for AIMP 5.40 x64";
        static TChar fullDesc[] = L"Single 1px red waveform line designed for dark/red AIMP skins.";
        switch (index) {
            case AIMP_PLUGIN_INFO_NAME: return name;
            case AIMP_PLUGIN_INFO_AUTHOR: return author;
            case AIMP_PLUGIN_INFO_SHORT_DESCRIPTION: return shortDesc;
            case AIMP_PLUGIN_INFO_FULL_DESCRIPTION: return fullDesc;
            default: return const_cast<TChar*>(L"");
        }
    }

public:
    HRESULT WINAPI QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (riid == IID_IUnknown) {
            *ppv = static_cast<IAIMPPlugin*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG WINAPI AddRef() override { return ++refs_; }

    ULONG WINAPI Release() override {
        ULONG n = --refs_;
        if (n == 0) delete this;
        return n;
    }

    TChar* WINAPI InfoGet(int Index) override {
        return text(Index);
    }

    LongWord WINAPI InfoGetCategories() override {
        return AIMP_PLUGIN_CATEGORY_VISUALS;
    }

    HRESULT WINAPI Initialize(IAIMPCore* Core) override {
        if (!Core) return E_INVALIDARG;

        core_ = Core;
        core_->AddRef();

        visualization_ = new RedLineVisualization();

        HRESULT hr = core_->RegisterExtension(
            IID_IAIMPServiceVisualizations,
            static_cast<IAIMPExtensionEmbeddedVisualization*>(visualization_)
        );

        if (FAILED(hr)) {
            visualization_->Release();
            visualization_ = nullptr;
            core_->Release();
            core_ = nullptr;
            return hr;
        }

        // Keep our own reference so Finalize can explicitly unregister it.
        return S_OK;
    }

    HRESULT WINAPI Finalize() override {
        if (core_ && visualization_) {
            core_->UnregisterExtension(
                static_cast<IAIMPExtensionEmbeddedVisualization*>(visualization_)
            );
            visualization_->Release();
            visualization_ = nullptr;
        }

        if (core_) {
            core_->Release();
            core_ = nullptr;
        }
        return S_OK;
    }

    void WINAPI SystemNotification(int, IUnknown*) override {}
};

extern "C" __declspec(dllexport)
HRESULT WINAPI AIMPPluginGetHeader(IAIMPPlugin** Header) {
    if (!Header) return E_POINTER;
    *Header = nullptr;

    try {
        *Header = new RedLinePlugin();
        return S_OK;
    } catch (...) {
        return E_OUTOFMEMORY;
    }
}
