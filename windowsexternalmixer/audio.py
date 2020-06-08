from ctypes import cast, POINTER, HRESULT
from ctypes.wintypes import BOOL, DWORD, LPWSTR, BYTE
from enum import IntEnum

import comtypes
import psutil
from comtypes import GUID, COMMETHOD, IUnknown
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume, IAudioSessionManager2, ISimpleAudioVolume


class Speakers:
    """
    Object representing the main speakers.

    Attributes:
        mm_device (POINTER): A pointer to the underlying IMMDevice interface for
            the speakers.
    """

    class State(IntEnum):
        """State that the speakers can be in."""

        ACTIVE = 1
        DISABLED = 2
        NOT_PRESENT = 4
        UNPLUGGED = 8

    def __init__(self):
        self.mm_device = AudioUtilities.GetSpeakers()

        void_audio_endpoint_volume = self.mm_device.Activate(IAudioEndpointVolume._iid_, comtypes.CLSCTX_ALL, None)
        self._audio_endpoint_volume = cast(void_audio_endpoint_volume, POINTER(IAudioEndpointVolume))

    @property
    def level(self):
        return round(self._audio_endpoint_volume.GetMasterVolumeLevelScalar() * 100)

    @level.setter
    def level(self, value):
        self._audio_endpoint_volume.SetMasterVolumeLevelScalar(value / 100, None)

    @property
    def is_muted(self):
        return bool(self._audio_endpoint_volume.GetMute())

    @is_muted.setter
    def is_muted(self, value):
        self._audio_endpoint_volume.SetMute(value, None)

    @property
    def state(self):
        """
        Return the state of the speakers.

        Returns:
            A State object.
        """
        return self.State(self.mm_device.GetState())


class IAudioSessionControl(IUnknown):
    _iid_ = GUID("{F4B1A599-7266-4319-A8CA-E70ACB11E8CD}")
    _methods_ = (
        # HRESULT GetState ([out] AudioSessionState *pRetVal);
        COMMETHOD([], HRESULT, "GetState", (["out"], POINTER(BYTE), "pRetVal")),
        # HRESULT GetDisplayName([out] LPWSTR *pRetVal);
        COMMETHOD([], HRESULT, "GetDisplayName", (["out"], POINTER(LPWSTR), "pRetVal")),
        # HRESULT SetDisplayName(
        # [in] LPCWSTR Value,
        # [in] LPCGUID EventContext);
        COMMETHOD([], HRESULT, "NotImpl2"),
        # HRESULT GetIconPath([out] LPWSTR *pRetVal);
        COMMETHOD([], HRESULT, "NotImpl3"),
        # HRESULT SetIconPath(
        # [in] LPCWSTR Value,
        # [in] LPCGUID EventContext);
        COMMETHOD([], HRESULT, "NotImpl4"),
        # HRESULT GetGroupingParam([out] GUID *pRetVal);
        COMMETHOD([], HRESULT, "NotImpl5"),
        # HRESULT SetGroupingParam(
        # [in] LPCGUID Grouping,
        # [in] LPCGUID EventContext);
        COMMETHOD([], HRESULT, "NotImpl6"),
        # HRESULT RegisterAudioSessionNotification(
        # [in] IAudioSessionEvents *NewNotifications);
        COMMETHOD([], HRESULT, "NotImpl7"),
        # HRESULT UnregisterAudioSessionNotification(
        # [in] IAudioSessionEvents *NewNotifications);
        COMMETHOD([], HRESULT, "NotImpl8"),
    )


class IAudioSessionControl2(IAudioSessionControl):
    _iid_ = GUID("{BFB7FF88-7239-4FC9-8FA2-07C950BE9C6D}")
    _methods_ = (
        # HRESULT GetSessionIdentifier([out] LPWSTR *pRetVal);
        COMMETHOD([], HRESULT, "GetSessionIdentifier", (["out"], POINTER(LPWSTR), "pRetVal")),
        # HRESULT GetSessionInstanceIdentifier([out] LPWSTR *pRetVal);
        COMMETHOD([], HRESULT, "GetSessionInstanceIdentifier", (["out"], POINTER(LPWSTR), "pRetVal")),
        # HRESULT GetProcessId([out] DWORD *pRetVal);
        COMMETHOD([], HRESULT, "GetProcessId", (["out"], POINTER(DWORD), "pRetVal")),
        # HRESULT IsSystemSoundsSession();
        COMMETHOD([], HRESULT, "IsSystemSoundsSession"),
        COMMETHOD([], HRESULT, "SetDuckingPreferences", (["in"], BOOL, "optOut")),
    )


class SessionNotFoundError(Exception):
    """Audio session was not found."""


class Session:
    """
    Object representing an audio session.

    Args:
         speakers (Speakers): The speakers that the session is playing out of.
         process_name (str): The name of the session's process.
    """

    class State(IntEnum):
        INACTIVE = 0
        ACTIVE = 1
        EXPIRED = 2

    def __init__(self, speakers, process_name):
        self._speakers = speakers

        void_audio_session_manager2 = speakers.mm_device.Activate(
            IAudioSessionManager2._iid_, comtypes.CLSCTX_ALL, None
        )
        audio_session_manager2 = cast(void_audio_session_manager2, POINTER(IAudioSessionManager2))
        audio_session_enumerator = audio_session_manager2.GetSessionEnumerator()
        for i in range(audio_session_enumerator.GetCount()):
            audio_session_control = audio_session_enumerator.GetSession(i).QueryInterface(IAudioSessionControl2)
            process = psutil.Process(audio_session_control.GetProcessId())
            if process.name() == process_name:
                self._audio_session_control = audio_session_control
                self._simple_audio_volume = audio_session_control.QueryInterface(ISimpleAudioVolume)
                break
        else:
            raise SessionNotFoundError(f"Audio session with process {process_name} was not found.")

    @property
    def level(self):
        """
        int: Current level of the volume control as a number between 0 and 100.

        If the level set is greater than the volume level of the speakers then
        the speakers will be raised to that level.
        """
        # GetMasterVolume returns the volume level relative to the main speaker volume.
        relative_level = self._simple_audio_volume.GetMasterVolume()
        return round(relative_level * self._speakers.level)

    @level.setter
    def level(self, value):
        if value > self._speakers.level:
            self._simple_audio_volume.SetMasterVolume(1, None)
            self._speakers.level = value
        else:
            self._simple_audio_volume.SetMasterVolume(value / self._speakers.level, None)

    @property
    def is_muted(self):
        return bool(self._simple_audio_volume.GetMute())

    @is_muted.setter
    def is_muted(self, value):
        self._simple_audio_volume.SetMute(value, None)

    @property
    def state(self):
        """
        Return the state of the audio session.

        Returns:
            A State object.
        """
        return self.State(self._audio_session_control.GetState())
